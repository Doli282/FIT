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
    struct ProblemPackWrapper;
    struct ProgtestSolverWrapper;
    struct CompanyWrapper;

    std::vector<std::shared_ptr<CompanyWrapper>> companyRegister;

    std::mutex mt_openProgtestSolver;
    std::shared_ptr<ProgtestSolverWrapper> openProgtestSolver;
    
    std::mutex mt_ptsQueue;
    std::condition_variable cv_ptsQueue;
    std::list<std::shared_ptr<ProgtestSolverWrapper>> ptsQueue;

    int NoThreads;
    std::vector<std::thread> workerThreads;
    void worker();
};

// TODO: COptimizer implementation goes here

struct COptimizer::ProblemPackWrapper
{
  ProblemPackWrapper(AProblemPack pack, CompanyWrapper & origCompany) : problemPack(pack), leftUnsolved(pack->m_Problems.size()), originalCompany(origCompany) {}
  AProblemPack problemPack;
  std::mutex mt_leftUnsolved;
  size_t leftUnsolved;
  CompanyWrapper & originalCompany;
};

struct COptimizer::ProgtestSolverWrapper
{
  ProgtestSolverWrapper(AProgtestSolver pts) : progtestSolver(pts){} 
  AProgtestSolver progtestSolver;
  std::vector<std::shared_ptr<ProblemPackWrapper>> originalProblemPacks;
};

struct COptimizer::CompanyWrapper
{
  CompanyWrapper(COptimizer & parent, ACompany origCompany) : optimizer(parent), company(origCompany){}
  COptimizer & optimizer;
  ACompany company;
  
  void startWorking();
  bool working = true;
  
  std::mutex mt_companyQueue;
  std::condition_variable cv_companyQueue;
  std::list<std::shared_ptr<ProblemPackWrapper>> companyQueue;
  
  std::thread recievingThread;
  std::thread submittingThread;
  void reciever();
  void submitter();
};

// start communication threads
void COptimizer::CompanyWrapper::startWorking()
{
  printf("company starts CTs\n");
  recievingThread = std::thread(&COptimizer::CompanyWrapper::reciever, this);
  submittingThread = std::thread(&COptimizer::CompanyWrapper::submitter, this);
  return;
}

void COptimizer::CompanyWrapper::reciever()
{
  printf("->RT-%d- : launching\n", this_thread::get_id());
  AProblemPack newPack;
  std::shared_ptr<ProblemPackWrapper> newProblemPack;
  while(newPack = company->waitForPack()) // run until nullpointer was given
  {
    printf("->RT-%d- : got new pack\n", this_thread::get_id());
    newProblemPack = std::make_shared<ProblemPackWrapper>(newPack, *this); // wrap in the wrapper
    
    // push problemPack to companyQueue to remember its rank
    printf("->RT-%d- : waiting for 'mt_companyQueue'\n", this_thread::get_id());
    mt_companyQueue.lock();
    printf("->RT-%d- : got 'mt_companyQueue'\n", this_thread::get_id());
    companyQueue.push_back(newProblemPack);
    mt_companyQueue.unlock();
    printf("->RT-%d- : unlocked 'mt_companyQueue'\n", this_thread::get_id());

    optimizer.mt_openProgtestSolver.lock(); // get unique access to openProgtestSolver
    printf("->RT-%d- : got 'mt_openProgtestSolver'\n", this_thread::get_id());
    for(size_t i = 0; i < newProblemPack->leftUnsolved; i++) // push all problems from the pack into the progtestSolver
    {
      // try to push problem to progtestSolver
    printf("->RT-%d- : try pushing into PTS, profit = %d\n", this_thread::get_id(), newPack->m_Problems.at(i)->m_MaxProfit);
      if(optimizer.openProgtestSolver->progtestSolver->addProblem(newPack->m_Problems.at(i)))
      {
    printf("->RT-%d- : push successful\n", this_thread::get_id());
        optimizer.openProgtestSolver->originalProblemPacks.push_back(newProblemPack);
      }
      else
      { // ProgtestSolver was full -> push to waiting queue, prepare new ProgtestSolver and push into the new one
    printf("->RT-%d- : push UNsuccessful -> create new PTS\n", this_thread::get_id());
        optimizer.mt_ptsQueue.lock();
    printf("->RT-%d- : got 'mt_ptsQueue'\n", this_thread::get_id());
        optimizer.ptsQueue.push_back(optimizer.openProgtestSolver);
        optimizer.mt_ptsQueue.unlock();
    printf("->RT-%d- : unlocked 'mt_ptsQueue' - notify 'cv_ptsQueue'\n", this_thread::get_id());
        optimizer.cv_ptsQueue.notify_one();
    printf("->RT-%d- : create new PTS\n", this_thread::get_id());
        optimizer.openProgtestSolver = std::make_shared<ProgtestSolverWrapper>(createProgtestSolver());
        optimizer.openProgtestSolver->progtestSolver->addProblem(newPack->m_Problems.at(i));
        optimizer.openProgtestSolver->originalProblemPacks.push_back(newProblemPack);
      }
    }
    // check if progtest solver is full
    printf("->RT-%d- : check if PTS is full\n", this_thread::get_id());
    if(!(optimizer.openProgtestSolver->progtestSolver->hasFreeCapacity()))
    {
    printf("->RT-%d- : PTS is full\n", this_thread::get_id());
        optimizer.mt_ptsQueue.lock();
    printf("->RT-%d- : got 'mt_ptsQueue'\n", this_thread::get_id());
        optimizer.ptsQueue.push_back(optimizer.openProgtestSolver);
        optimizer.mt_ptsQueue.unlock();
    printf("->RT-%d- : unlocked 'mt_ptsQueue' - notify 'cv_ptsQueue'\n", this_thread::get_id());
        optimizer.cv_ptsQueue.notify_one();
    printf("->RT-%d- : create new PTS\n", this_thread::get_id());
        optimizer.openProgtestSolver = std::make_shared<ProgtestSolverWrapper>(createProgtestSolver());
    }
    printf("->RT-%d- : unlock 'mt_openProgtestSolver'\n", this_thread::get_id());
    optimizer.mt_openProgtestSolver.unlock();
  }
  // nullptr was given -> end of input
  // check out at the end of the day, if is was the last company, push the progtestSolver to queue
  printf("->RT-%d- : stop working\n", this_thread::get_id());
  working = false;
  printf("->RT-%d- : finishing...\n", this_thread::get_id());
  return;
}

void COptimizer::CompanyWrapper::submitter()
{
  printf("<-ST-%d- : launching\n", this_thread::get_id());
  AProblemPack finishedPack;
  while(working || (!companyQueue.empty())) // work till the shift is over - i. e. reciever thread stops running
  {
    // wait until woken up by worker threads that new pack is finished
    printf("<-ST-%d- : wait for 'ul_companyQueue'\n", this_thread::get_id());
    std::unique_lock<mutex> ul_companyQueue (mt_companyQueue);
    printf("<-ST-%d- : sleep on 'cv_companyQueue'\n", this_thread::get_id());
    cv_companyQueue.wait(ul_companyQueue);
    printf("<-ST-%d- : wake from 'cv_comapnyQueue'\n", this_thread::get_id());


    while(!companyQueue.empty()) // check if there are problemPacks in the queue
    {
    printf("<-ST-%d- : companyQueue is not empty\n", this_thread::get_id());
      if(companyQueue.front()->leftUnsolved == 0) // has the first ProblemPack been finished
      {
    printf("<-ST-%d- : first ProblemPack is solved\n", this_thread::get_id());
        //mt_companyQueue.lock();
    printf("<-ST-%d- : got 'mt_companyQueue'\n", this_thread::get_id());
        finishedPack = companyQueue.front()->problemPack;
        companyQueue.pop_front();
        mt_companyQueue.unlock();
    printf("<-ST-%d- : unlock 'mt_companyQueue\n", this_thread::get_id());
        company->solvedPack(finishedPack); // submit finished problemPack
    printf("<-ST-%d- : pack solved\n", this_thread::get_id());
      }
      else
      { // the first ProblemPack has not been finished yet
    printf("<-ST-%d- : first ProblemPack is still unsolved\n", this_thread::get_id());
        break;
      }
    }
    printf("<-ST-%d- : company queue is empty\n", this_thread::get_id());
  }
  printf("<-ST-%d- : finishing....\n", this_thread::get_id());
  return;
}

void COptimizer::worker()
{
  std::shared_ptr<ProgtestSolverWrapper> ownProgtestSolver;
  printf("WT-%d- : launching\n", this_thread::get_id());
  while(true) // work until there is nothing to solve and reciever stopped working
  {
    printf("WT-%d- : new round\n", this_thread::get_id());
    std::unique_lock<mutex> ul_ptsQueue (mt_ptsQueue);
    while(ptsQueue.empty()) // wait until there are ProgtestSolvers in the queue
    {
    printf("WT-%d- : ptsQueue is empty\n", this_thread::get_id());
    printf("WT-%d- : waiting for 'ul_ptsQueue'\n", this_thread::get_id());
    printf("WT-%d- : sleep on 'cv_ptsQueue\n", this_thread::get_id());
      cv_ptsQueue.wait(ul_ptsQueue);
    printf("WT-%d- : woken up from 'cv_ptsQueue'\n", this_thread::get_id());
    }

    printf("WT-%d- : go to work -> wait for 'mt_ptsQueue'\n", this_thread::get_id());
    // pick a ProgtestSolver up from the waiting queue
    printf("WT-%d- : got 'mt_ptsQueue'\n", this_thread::get_id());
    ownProgtestSolver = ptsQueue.front();
    ptsQueue.pop_front();
    ul_ptsQueue.unlock();
    printf("WT-%d- : unlocked 'mt_ptsQueue'\n", this_thread::get_id());

    if(ownProgtestSolver == nullptr) break; // finishes when reaching nullptr

    ownProgtestSolver->progtestSolver->solve(); // solve the ProgtestSolver
    printf("WT-%d- : solved ProgtestSolver\n", this_thread::get_id());
    for(auto problemPack : ownProgtestSolver->originalProblemPacks) // inform ProblemPacks that their Problem has been solved
    {
    printf("WT-%d- : wait for 'mt_leftUnsolved'\n", this_thread::get_id());
      problemPack->mt_leftUnsolved.lock();
    printf("WT-%d- : got 'mt_leftUnsolved'\n", this_thread::get_id());
      if(--(problemPack->leftUnsolved) == 0) // if the whole Pack has been solved, wake up the submitter thread
      {
    printf("WT-%d- : whole pack is solved -> wake 'cv_companyQueue'\n", this_thread::get_id());
        int i = 0;
        for(auto problem : problemPack->problemPack->m_Problems)
        {
    printf("WT-%d- : %d/%d new profit = %d\n", this_thread::get_id(), i++, problemPack->problemPack->m_Problems.size(), problem->m_MaxProfit);
        }
        problemPack->originalCompany.cv_companyQueue.notify_one();
      }
      problemPack->mt_leftUnsolved.unlock();
    printf("WT-%d- : unlock 'mt_leftUnsolved'\n", this_thread::get_id());
    }
    printf("WT-%d- : PTS finished\n", this_thread::get_id());
  }
    printf("WT-%d- : finishing...\n", this_thread::get_id());
  return;
}

void COptimizer::addCompany(ACompany company)
{
  printf("Add company\n");
  companyRegister.push_back(std::make_shared<CompanyWrapper>(*this, company)); // register the company
  return;
}

void COptimizer::start(int threadCount)
{
  printf("set first Solver\n");
  openProgtestSolver = std::make_shared<ProgtestSolverWrapper>(createProgtestSolver()); // create first ProgtestSolver
  printf("launch CTs\n");
  for(auto company : companyRegister) // start communication threads in companies
  {
    company->startWorking();
  }
  printf("launch WTs\n");
  for(NoThreads = 0; NoThreads < threadCount; NoThreads++) // start worker threads
  {
    workerThreads.push_back(thread(&COptimizer::worker, this));
  }
  return;
}

void COptimizer::stop(void)
{
  printf("STOP\n");
  //wait for recievers, wokrers, submitters
  printf("wait for RT\n");
  for(auto company : companyRegister)
  {
    company->recievingThread.join();
  }
  ptsQueue.push_back(openProgtestSolver);
  printf("push nullptrs in PTSqueue\n");
  for(int i = 0; i < NoThreads; i++)
  {
    ptsQueue.push_back(nullptr);
  }
  cv_ptsQueue.notify_all();
  printf("wait for WT\n");
  for(auto & worker : workerThreads)
  {
    worker.join();
  }
  printf("wait for ST\n");
  for(auto company : companyRegister)
  {
    company->submittingThread.join();
  }
  printf("END\n");
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
