#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <climits>
#include <cfloat>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <string>
#include <vector>
#include <array>
#include <iterator>
#include <set>
#include <list>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <stack>
#include <deque>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <stdexcept>
#include <condition_variable>
#include <pthread.h>
#include <semaphore.h>
#include "progtest_solver.h"
#include "sample_tester.h"
using namespace std;
#endif /* __PROGTEST__ */ 

#define DEBUG 0
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
class COptimizer 
{
  public:
    static bool                        usingProgtestSolver                     ( void )
    {
      return true;
    }
    static void                        checkAlgorithm                          ( AProblem                              problem )
    {
      // dummy implementation if usingProgtestSolver() returns true
    }
    void                               start                                   ( int                                   threadCount );
    void                               stop                                    ( void );
    void                               addCompany                              ( ACompany                              company );
  private:
  // wrappers on given classes
    struct ProblemPackWrapper;
    struct ProgtestSolverWrapper;
    struct CompanyWrapper;

    // where pointers to all companies are held
    std::vector<std::shared_ptr<CompanyWrapper>> companyRegister;

    // currently open ProgtestSolver filled by recievers
    std::mutex mt_openProgtestSolver;
    std::shared_ptr<ProgtestSolverWrapper> openProgtestSolver;
    
    // waiting queue for ProgtestSolvers to be solved by WorkerThreads
    std::mutex mt_ptsQueue;
    std::condition_variable cv_ptsQueue;
    std::list<std::shared_ptr<ProgtestSolverWrapper>> ptsQueue;

    // Worker threads are saved in vector
    int NoThreads;
    std::vector<std::thread> workerThreads;
    void worker();
};

// TODO: COptimizer implementation goes here

struct COptimizer::ProblemPackWrapper
{
  ProblemPackWrapper(AProblemPack pack, CompanyWrapper & origCompany) : problemPack(pack), leftUnsolved(pack->m_Problems.size()), NoProblemsInPack(leftUnsolved), originalCompany(origCompany) {}
  AProblemPack problemPack; // actual problemPack
  std::mutex mt_leftUnsolved;
  size_t leftUnsolved; // problems left to be solved
  size_t NoProblemsInPack; // number of problems in pack
  CompanyWrapper & originalCompany; // reference to original company
};

struct COptimizer::ProgtestSolverWrapper
{
  ProgtestSolverWrapper(AProgtestSolver pts) : progtestSolver(pts){} 
  AProgtestSolver progtestSolver; // actual progtestSolver
  std::vector<std::shared_ptr<ProblemPackWrapper>> originalProblemPacks; // vector of pointers to problemPack which the problems are originally from
};

struct COptimizer::CompanyWrapper
{
  CompanyWrapper(COptimizer & parent, ACompany origCompany) : optimizer(parent), company(origCompany){}
  COptimizer & optimizer; // reference to parent optimizer -> used for accessing "global" variables
  ACompany company; // actual company
  
  void startWorking(); // initialization of communication threads
  bool working = true; // is the reciever still recieving -> information given to the submitter
  
  bool submitAtAllCost = false; // are the workingThreads still running? - if not, submit all is left

  // waiting queue of problemPack of given company
  std::mutex mt_companyQueue;
  std::condition_variable cv_companyQueue;
  std::list<std::shared_ptr<ProblemPackWrapper>> companyQueue;
  
  std::thread recievingThread;
  std::thread submittingThread;
  void reciever(); // function for recieverThread
  void submitter(); // function for submitterThread
};

// start communication threads
void COptimizer::CompanyWrapper::startWorking()
{
  if(DEBUG) ;//printf("company starts CTs\n");
  recievingThread = std::thread(&COptimizer::CompanyWrapper::reciever, this);
  submittingThread = std::thread(&COptimizer::CompanyWrapper::submitter, this);
  return;
}

void COptimizer::CompanyWrapper::reciever()
{
  if(DEBUG) ;//printf("->RT-%ld- : launching\n", this_thread::get_id());
  AProblemPack newPack; // own instance of new recieved pack
  std::shared_ptr<ProblemPackWrapper> newProblemPack; // own instance of Pack Wrapper for the newPack
  while(newPack = company->waitForPack()) // run until nullpointer was given and recieve new packs
  {
    if(DEBUG) ;//printf("->RT-%ld- : got new pack\n", this_thread::get_id());
    newProblemPack = std::make_shared<ProblemPackWrapper>(newPack, *this); // wrap in the wrapper
    
    // push problemPack to companyQueue to remember its rank
    if(DEBUG) ;//printf("->RT-%ld- : waiting for 'mt_companyQueue'\n", this_thread::get_id());
    
    // put the new pack in comapny waiting list
    mt_companyQueue.lock();
    if(DEBUG) ;//printf("->RT-%ld- : got 'mt_companyQueue'\n", this_thread::get_id());
    companyQueue.push_back(newProblemPack);
    mt_companyQueue.unlock();
    if(DEBUG) ;//printf("->RT-%ld- : unlocked 'mt_companyQueue'\n", this_thread::get_id());

    optimizer.mt_openProgtestSolver.lock(); // get unique access to openProgtestSolver
    if(DEBUG) ;//printf("->RT-%ld- : got 'mt_openProgtestSolver'\n", this_thread::get_id());
    for(size_t i = 0; i < newProblemPack->NoProblemsInPack;) // push all problems from the pack into the progtestSolver
    {
      // try to push problem to progtestSolver
      if(DEBUG) ;//printf("->RT-%ld- : try pushing into PTS, profit = %d\n", this_thread::get_id(), newPack->m_Problems.at(i)->m_MaxProfit);
      if(optimizer.openProgtestSolver->progtestSolver->addProblem(newPack->m_Problems.at(i)))
      {
        optimizer.openProgtestSolver->originalProblemPacks.push_back(newProblemPack);
        i++; // move to the next problem only when successfully pushed
        if(DEBUG) ;//printf("->RT-%ld- : push successful, i=%ld / %ld\n", this_thread::get_id(), i, newProblemPack->NoProblemsInPack);
      }
      
      // check if progtest solver is full, if so, push it to ptsQueue and wake up workerThread
      if(DEBUG) ;//printf("->RT-%ld- : check if PTS is full\n", this_thread::get_id());
      if(!(optimizer.openProgtestSolver->progtestSolver->hasFreeCapacity()))
      {
        if(DEBUG) ;//printf("->RT-%ld- : PTS is full\n", this_thread::get_id());
        optimizer.mt_ptsQueue.lock();
        if(DEBUG) ;//printf("->RT-%ld- : got 'mt_ptsQueue'\n", this_thread::get_id());
        optimizer.ptsQueue.push_back(optimizer.openProgtestSolver);
        optimizer.mt_ptsQueue.unlock();
        if(DEBUG) ;//printf("->RT-%ld- : unlocked 'mt_ptsQueue' - notify 'cv_ptsQueue'\n", this_thread::get_id());
        optimizer.cv_ptsQueue.notify_one();
        if(DEBUG) ;//printf("->RT-%ld- : create new PTS\n", this_thread::get_id());
        optimizer.openProgtestSolver = std::make_shared<ProgtestSolverWrapper>(createProgtestSolver());
      }
    }
    if(DEBUG) ;//printf("->RT-%ld- : unlock 'mt_openProgtestSolver'\n", this_thread::get_id());
    optimizer.mt_openProgtestSolver.unlock();
    if(DEBUG)
    { // debug waiting for new instances
      for(int i = 0; i < 3; i++)
      {
        ;//printf("->RT-%ld- : going to sleep (%d), in queue (%ld), in PTSqueue (%ld), space in pts? %d\n", this_thread::get_id(), i, companyQueue.size(), optimizer.ptsQueue.size(), optimizer.openProgtestSolver->progtestSolver->hasFreeCapacity());
        this_thread::sleep_for(chrono::seconds(1)); 
      }
    }
  }
  // nullptr was given -> end of input -> stop working
  if(DEBUG) ;//printf("->RT-%ld- : stop working\n", this_thread::get_id());
  working = false;
  if(DEBUG) ;//printf("->RT-%ld- : finishing...\n", this_thread::get_id());
  return;
}

void COptimizer::CompanyWrapper::submitter()
{
  if(DEBUG) ;//printf("<-ST-%ld- : launching\n", this_thread::get_id());
  AProblemPack finishedPack; // own instance of ProblemPack that is ready for submission
  while(working || (!companyQueue.empty())) // work till the shift is over - i. e. reciever thread stops running - and then check for left Pack in queue
  {
    // wait until woken up by worker threads that new pack is finished
    if(DEBUG) ;//printf("<-ST-%ld- : wait for 'ul_companyQueue'\n", this_thread::get_id());
    std::unique_lock<mutex> ul_companyQueue (mt_companyQueue);
    if(DEBUG) ;//printf("<-ST-%ld- : sleep on 'cv_companyQueue'\n", this_thread::get_id());
    cv_companyQueue.wait(ul_companyQueue);
    if(DEBUG) ;//printf("<-ST-%ld- : wake from 'cv_comapnyQueue'\n", this_thread::get_id());


    while(!companyQueue.empty()) // check if there are problemPacks in the queue
    {
      if(DEBUG) ;//printf("<-ST-%ld- : companyQueue is not empty\n", this_thread::get_id());
      if(companyQueue.front()->leftUnsolved == 0 || submitAtAllCost) // has the first ProblemPack been finished
      {
        if(DEBUG) ;//printf("<-ST-%ld- : first ProblemPack is solved\n", this_thread::get_id());
        if(DEBUG) ;//printf("<-ST-%ld- : got 'mt_companyQueue'\n", this_thread::get_id());
        // take the first pack in the queue
        finishedPack = companyQueue.front()->problemPack;
        companyQueue.pop_front();
        mt_companyQueue.unlock();
        if(DEBUG) ;//printf("<-ST-%ld- : unlock 'mt_companyQueue\n", this_thread::get_id());
        
        if(DEBUG && submitAtAllCost)
        {
          for(auto pack : finishedPack->m_Problems)
          {
            ;//printf("<-ST-%ld- : force submit\n", this_thread::get_id());
          }
        }

        company->solvedPack(finishedPack); // submit finished problemPack
        if(DEBUG) ;//printf("<-ST-%ld- : pack solved\n", this_thread::get_id());
      }
      else
      { // the first ProblemPack has not been finished yet
        if(DEBUG) ;//printf("<-ST-%ld- : first ProblemPack is still unsolved (%ld)\n", this_thread::get_id(), companyQueue.front()->leftUnsolved);
        break;
      }
    }
    if(DEBUG) ;//printf("<-ST-%ld- : company queue is empty\n", this_thread::get_id());
  }
  if(DEBUG) ;//printf("<-ST-%ld- : finishing....\n", this_thread::get_id());
  return;
}

void COptimizer::worker()
{
  std::shared_ptr<ProgtestSolverWrapper> ownProgtestSolver; // own instance of ProgtestSolver ready to be solved
  if(DEBUG) ;//printf("WT-%ld- : launching\n", this_thread::get_id());
  while(true) // work until there is nothing to solve and reciever stopped working
  {
    if(DEBUG) ;//printf("WT-%ld- : new round\n", this_thread::get_id());
    std::unique_lock<mutex> ul_ptsQueue (mt_ptsQueue);
    while(ptsQueue.empty()) // wait until there are ProgtestSolvers in the queue -> woken up by recievers
    {
      if(DEBUG) ;//printf("WT-%ld- : ptsQueue is empty\n", this_thread::get_id());
      if(DEBUG) ;//printf("WT-%ld- : waiting for 'ul_ptsQueue'\n", this_thread::get_id());
      if(DEBUG) ;//printf("WT-%ld- : sleep on 'cv_ptsQueue\n", this_thread::get_id());
      cv_ptsQueue.wait(ul_ptsQueue);
      if(DEBUG) ;//printf("WT-%ld- : woken up from 'cv_ptsQueue'\n", this_thread::get_id());
    }

    if(DEBUG) ;//printf("WT-%ld- : go to work -> wait for 'mt_ptsQueue'\n", this_thread::get_id());
    // pick a ProgtestSolver up from the waiting queue
    if(DEBUG) ;//printf("WT-%ld- : got 'mt_ptsQueue'\n", this_thread::get_id());
    ownProgtestSolver = ptsQueue.front();
    ptsQueue.pop_front();
    ul_ptsQueue.unlock();
    if(DEBUG) ;//printf("WT-%ld- : unlocked 'mt_ptsQueue'\n", this_thread::get_id());

    if(ownProgtestSolver == nullptr) break; // finishes when reaching nullptr

    ownProgtestSolver->progtestSolver->solve(); // solve the ProgtestSolver
    if(DEBUG) ;//printf("WT-%ld- : solved ProgtestSolver\n", this_thread::get_id());
    for(auto problemPack : ownProgtestSolver->originalProblemPacks) // inform ProblemPacks that their Problem has been solved
    {
      if(DEBUG) ;//printf("WT-%ld- : wait for 'mt_leftUnsolved'\n", this_thread::get_id());
      problemPack->mt_leftUnsolved.lock();
      if(DEBUG) ;//printf("WT-%ld- : got 'mt_leftUnsolved'\n", this_thread::get_id());
      if((--(problemPack->leftUnsolved)) == 0) // decrease the number of unsolved problems in the packs -> if the whole Pack has been solved, wake up the submitter thread
      {
        if(DEBUG)
        {
          ;//printf("WT-%ld- : whole pack is solved -> wake 'cv_companyQueue'\n", this_thread::get_id());
          //int i = 1;
          for(auto problem : problemPack->problemPack->m_Problems)
          {
            ;//printf("WT-%ld- : %d/%ld new profit = %d\n", this_thread::get_id(), i++, problemPack->problemPack->m_Problems.size(), problem->m_MaxProfit);
          }
        }
        problemPack->originalCompany.cv_companyQueue.notify_one();
      }
      problemPack->mt_leftUnsolved.unlock();
      if(DEBUG) ;//printf("WT-%ld- : unlock 'mt_leftUnsolved'\n", this_thread::get_id());
    }
    if(DEBUG) ;//printf("WT-%ld- : PTS finished\n", this_thread::get_id());
  }
  if(DEBUG) ;//printf("WT-%ld- : finishing...\n", this_thread::get_id());
  return;
}

// register new company
void COptimizer::addCompany(ACompany company)
{
  if(DEBUG) ;//printf("Add company\n");
  companyRegister.push_back(std::make_shared<CompanyWrapper>(*this, company)); // register the company
  return;
}

// start working process -> launch threads
void COptimizer::start(int threadCount)
{
  if(DEBUG) ;//printf("set first Solver\n");
  openProgtestSolver = std::make_shared<ProgtestSolverWrapper>(createProgtestSolver()); // create first ProgtestSolver
  if(DEBUG) ;//printf("launch CTs\n");
  for(auto company : companyRegister) // start communication threads in companies
  {
    company->startWorking();
  }
  if(DEBUG) ;//printf("launch WTs\n");
  for(NoThreads = 0; NoThreads < threadCount; NoThreads++) // start worker threads
  {
    workerThreads.push_back(thread(&COptimizer::worker, this));
  }
  return;
}

// stop working -> wait for threads till they stop running
void COptimizer::stop(void)
{
  if(DEBUG) ;//printf("STOP\n");
  if(DEBUG) ;//printf("wait for RT\n");
  //wait for recievers, wokrers, submitters
  for(auto company : companyRegister)
  {
    company->recievingThread.join();
  }
  // submit the last progtestSolver, push nullptrs and wake all workers
  ptsQueue.push_back(openProgtestSolver);
  if(DEBUG) ;//printf("push nullptrs in PTSqueue\n");
  mt_ptsQueue.lock();
  for(int i = 0; i < NoThreads; i++)
  {
    ptsQueue.push_back(nullptr);
  }
  mt_ptsQueue.unlock();
  cv_ptsQueue.notify_all();

  if(DEBUG) ;//printf("wait for WT\n");
  for(auto & worker : workerThreads)
  {
    worker.join();
  }
  if(DEBUG) ;//printf("wait for ST\n");
  for(auto company : companyRegister)
  { // wake all submitters to finish their job 
    company->submitAtAllCost = true;
    company->cv_companyQueue.notify_all();
    company->submittingThread.join();
  }
  if(DEBUG) ;//printf("END\n");
  return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef __PROGTEST__
int                                    main                                    ( void )
{
  COptimizer optimizer;
  ACompanyTest  company = std::make_shared<CCompanyTest> ();
  optimizer . addCompany ( company );
  optimizer . start ( 4 );
  optimizer . stop  ();
  if ( ! company -> allProcessed () )
    throw std::logic_error ( "(some) problems were not correctly processsed" );
  return 0;  
}
#endif /* __PROGTEST__ */ 
