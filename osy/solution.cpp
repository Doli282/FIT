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
    // constructor without parameters
    COptimizer()
    {
      // initialize structures
    }
    static bool                        usingProgtestSolver                     ( void )
    {
      return true; // return true if using prepared solver; false when implementing my own
    }
    static void                        checkAlgorithm                          ( AProblem                              problem )
    {
      // dummy implementation if usingProgtestSolver() returns true
      // implement when using my own algorithm as bonus
    }
    void                               start                                   ( int                                   threadCount );
    void                               stop                                    ( void );
    void                               addCompany                              ( ACompany                              company );
  private:
  // ------------------------------------
  struct MyProblemPack;
  struct MyProgtestSolver;
  struct MyCompany;
  // ------------------------------------
  std::vector<MyCompany> companyRegister;
  // ------------------------------------
  std::list<std::shared_ptr<MyProblemPack>> globalWaitingQueue;
  std::list<std::shared_ptr<MyProblemPack>>::iterator iteratorToUnprocessedPacks = globalWaitingQueue.end();
  std::shared_ptr<MyProgtestSolver> currentSolver;
  // ------------------------------------
  bool working = true;
  int NoThreads;
  std::vector<thread> workerThreads;
  std::condition_variable cv_globalQueue;
  std::mutex mt_globalQueue;
  std::mutex mt_add;
  void worker();
};

// TODO: COptimizer implementation goes here
// --------------------------------------------------------------------------
struct COptimizer::MyProblemPack
{
  MyProblemPack(AProblemPack problemPack, sem_t * owner) : pack(problemPack), ownerCompany(owner), leftToProcess(problemPack->m_Problems.size()), leftToBeSolved(leftToProcess){}
  AProblemPack pack;
  // company tag
  sem_t * ownerCompany;
  //condition_variable * ownerCompany;
  // left to process
  size_t leftToProcess;
  // left to be solved
  size_t leftToBeSolved;
};
// --------------------------------------------------------------------------
struct COptimizer::MyProgtestSolver
{
  MyProgtestSolver(AProgtestSolver aProgtestSolver) : progtestSolver(aProgtestSolver){}
  bool iteratorSet = false;
  std::list<std::shared_ptr<MyProblemPack>>::iterator iteratorToProcessedPacks;
  AProgtestSolver progtestSolver;
};
// --------------------------------------------------------------------------
struct COptimizer::MyCompany
{
  MyCompany(COptimizer & optimizer, ACompany aCompany) : parent(optimizer), company(aCompany){ sem_init(&sem_submit, 0,0);} 
  MyCompany(MyCompany && other) = default;
  ~MyCompany() {sem_destroy(&sem_submit);}
  COptimizer & parent;
  ACompany company; 
  bool recieving = true;
  //std::unique_ptr<std::thread> recievingThread;
  std::thread recievingThread;
  std::thread submittingThread;
  //std::mutex mt_submit;
  //std::condition_variable cv_submit;
  sem_t sem_submit;
  std::deque<std::shared_ptr<MyProblemPack>> companyWaitingQueue;

  void start();
  void reciever();
  void submitter();
};

void COptimizer::MyCompany::start()
{
  printf("Initializing communication threads\n");
  recievingThread = std::thread(&COptimizer::MyCompany::reciever, this);
  submittingThread = std::thread(&MyCompany::submitter, this);
}

// reciever thread main function
void COptimizer::MyCompany::reciever()
{
  printf("->RT-%d- : launching\n", this_thread::get_id());
  // take new ProblemPacks and push_back them to the waiting queue
  // when new pack was recieved signalise worker threads
  // stop when nullptr was recieved = EOF
  AProblemPack newProblemPack;
  while(newProblemPack = company->waitForPack())
  {
    printf("->RT-%d- : got new pack\n", this_thread::get_id());
    std::shared_ptr<MyProblemPack> newMyProblemPack = std::make_shared<MyProblemPack>(newProblemPack, &sem_submit);
    // may be critical
    companyWaitingQueue.push_back(newMyProblemPack);
    // probably is critical
    printf("->RT-%d- : waiting for 'mt_globalQueue'\n", this_thread::get_id());
    std::lock_guard<mutex> lg_insertInBuffer (parent.mt_globalQueue);
    printf("->RT-%d- : got 'mt_globalQueue'\n", this_thread::get_id());
    parent.globalWaitingQueue.push_back(newMyProblemPack);
    // wake workers up
    if(parent.iteratorToUnprocessedPacks == parent.globalWaitingQueue.end())
    {
      printf("->RT-%d- : set iterator on unporcessed Packs\n", this_thread::get_id());
      parent.iteratorToUnprocessedPacks = std::prev(parent.globalWaitingQueue.end());
      printf("->RT-%d- : waking one on 'cv_globalQueue'\n", this_thread::get_id());
      parent.cv_globalQueue.notify_one();
    }
    // leaves critical
  }
  // finish workers
  recieving = false;
  parent.cv_globalQueue.notify_all();
  printf("->RT-%d- : finishing\n", this_thread::get_id());
  return;
}

// submitter thread main fucniton
void COptimizer::MyCompany::submitter()
{
    printf("<-ST-%d- : launching\n", this_thread::get_id());
  // check first ProblemPack in the queue if it is completed
  // if solved then submit it
  // otherwise sleep and wait for signal
  std::shared_ptr<MyProblemPack> firstPackInLine;
  while(true)
  {
    if(!companyWaitingQueue.empty())
    {
    printf("<-ST-%d- : looking for completed problemPack\n", this_thread::get_id());
      // may be critical
      firstPackInLine = companyWaitingQueue.front();
      if(firstPackInLine->leftToBeSolved == 0)
      {
        printf("<-ST-%d- : submitting problemPack\n", this_thread::get_id());
        company->solvedPack(firstPackInLine->pack);
        // may be critical
        companyWaitingQueue.pop_front();
        // leaves critical
    printf("<-ST-%d- : problemPack submitted'\n", this_thread::get_id());
        continue;
      }
    }  
    else if(!recieving)
    {
    printf("<-ST-%d- : stopped submitting\n", this_thread::get_id());

      break;
    }
    
    // sleep - woken up by workers
    printf("<-ST-%d- : sleeping on 'sem_submit'\n", this_thread::get_id()); // WRONG SLEEP -> LOCK
    sem_wait(&sem_submit);
    //std::unique_lock<mutex> ul_submit(mt_submit);
    //cv_submit.wait(ul_submit);
    //ul_submit.unlock();
  }
  // finish when queue is empty and reciever has stopped recieving
    printf("<-ST-%d- : finishing\n", this_thread::get_id());
  return;
}
// --------------------------------------------------------------------------
// worker thread main function
void COptimizer::worker()
{
    printf("WT-%d- : launching\n", this_thread::get_id());
  // check global waiting queue
  // put new problems to problem solver
  // if problem solver is full, launch problem solver
  // after problem solver finishes, mark problems as solved and wake dedicated submitter
  std::unique_lock<mutex> ul_addProblem (mt_add, std::defer_lock);
  while(true)
  {
    printf("WT-%d- : waiting for 'ul_addProblem'\n", this_thread::get_id());
    ul_addProblem.lock();
    printf("WT-%d- : got 'ul_addProblem'\n", this_thread::get_id());
    while(iteratorToUnprocessedPacks == globalWaitingQueue.end() && working)
    {
      printf("WT-%d- : sleeping on 'cv_globalQueue'\n", this_thread::get_id());
      cv_globalQueue.wait(ul_addProblem);
      printf("WT-%d- : awake after 'cv_globalQueue'\n", this_thread::get_id());
    }
    if(!working)
    {
      printf("WT-%d- : stop working\n", this_thread::get_id());
      if(ul_addProblem) ul_addProblem.unlock();
      break;
    }
    // check global waiting queue
/*       printf("WT-%d- : in front of while\n", this_thread::get_id());
      printf("WT-%d- : locking  'ul_addProblem'\n", this_thread::get_id());
      ul_addProblem.lock();
      printf("WT-%d- : got 'ul_addProblem'\n", this_thread::get_id());
    while(iteratorToUnprocessedPacks == globalWaitingQueue.end())
    {
      // sleep till woken up by recievers
      printf("WT-%d- : unlocking  'ul_addProblem'\n", this_thread::get_id());
      ul_addProblem.unlock();
      printf("WT-%d- : locking 'mt_globalQueue'\n", this_thread::get_id());
      std::unique_lock<mutex> ul_globalQueue (mt_globalQueue);
      printf("WT-%d- : sleeping on 'cv_global'\n", this_thread::get_id());
      cv_globalQueue.wait(ul_globalQueue);
      printf("WT-%d- : locking  'ul_addProblem'\n", this_thread::get_id());
      ul_addProblem.lock();
      printf("WT-%d- : got 'ul_addProblem'\n", this_thread::get_id());
    }
      printf("WT-%d- : behind while\n", this_thread::get_id()); */
    printf("---- %d packs to precess in global queue\n", globalWaitingQueue.size());
    std::shared_ptr<MyProblemPack> packToProcess = *iteratorToUnprocessedPacks; //  INVALID ITERATOR HANDLING - NEKDE JE TU CHYBA A PRISTUPUJU PAK NA NEPLATNY ITERATOR (A PAK SE ZAZRACNE SPRAVI SAMA.....)
    if(packToProcess->leftToProcess > 0)
    {
      printf("WT-%d- : adding Problem to ProblemSolver\n", this_thread::get_id());
      // add problem to ProgtestSolver
      packToProcess->leftToProcess--;
      currentSolver->progtestSolver->addProblem(packToProcess->pack->m_Problems.at(packToProcess->leftToProcess));
      
      // check for set iterator to the first processed pack
      if(!currentSolver->iteratorSet)
      {
      printf("WT-%d- : set iterator for problemSolver\n", this_thread::get_id());
        currentSolver->iteratorToProcessedPacks = iteratorToUnprocessedPacks;
        currentSolver->iteratorSet = true;
      }

      // check if the solver is full
      if(!(currentSolver->progtestSolver->hasFreeCapacity()))
      {
      printf("WT-%d- : problemSolver is full\n", this_thread::get_id());  
        // create new solver for other workers
        std::shared_ptr<MyProgtestSolver> fullSolver = currentSolver;
        currentSolver = std::make_shared<MyProgtestSolver>(createProgtestSolver());
      printf("WT-%d- : unlock 'ul_addProblem' 1\n", this_thread::get_id());
        ul_addProblem.unlock();
        // solve the problems
      printf("WT-%d- : solve problemSolver\n", this_thread::get_id());

        size_t solved = fullSolver->progtestSolver->solve();
        printf("WT-%d- : problemSolver solved\n", this_thread::get_id());
        // mark problems as solved
        while(solved > 0)
        {
          if(solved >= ((*(fullSolver->iteratorToProcessedPacks))->leftToBeSolved))
          {
            solved -= ((*(fullSolver->iteratorToProcessedPacks))->leftToBeSolved);
            ((*(fullSolver->iteratorToProcessedPacks))->leftToBeSolved) = 0;
            // wake dedicated company
            printf("WT-%d- : wake ST\n", this_thread::get_id());
            sem_post((*(fullSolver->iteratorToProcessedPacks))->ownerCompany);
            //(*(fullSolver->iteratorToProcessedPacks))->ownerCompany->notify_all();
      printf("WT-%d- : erase from globalWaitingQueue\n", this_thread::get_id());
            fullSolver->iteratorToProcessedPacks = globalWaitingQueue.erase(fullSolver->iteratorToProcessedPacks);
          }
          else
          {
      printf("WT-%d- : ProblemPack only partialy solved\n", this_thread::get_id());
            ((*(fullSolver->iteratorToProcessedPacks))->leftToBeSolved) -= solved;
            break; // solved = 0;
          }
        }
      }
        printf("WT-%d- : check for 'ul_addProblem' 2\n", this_thread::get_id());
      
      if(ul_addProblem)
      {
        printf("WT-%d- : unlock 'ul_addProblem' 2\n", this_thread::get_id());
        ul_addProblem.unlock();
      }
      
    }
    // pack is being processed, move to next one
    else
    {
      printf("WT-%d- : set iterator for unprocessedPacks\n", this_thread::get_id());
      ++iteratorToUnprocessedPacks;
      printf("WT-%d- : unlock 'ul_addProblem' 3\n", this_thread::get_id());
      ul_addProblem.unlock();
    }
  }
      printf("WT-%d- : finishing\n", this_thread::get_id());
  return;
}



// add new instance of company
void COptimizer::addCompany(ACompany company)
{
  // add new instance of company to data structure (vector?)
  printf("Add company\n");
  companyRegister.emplace_back(*this, company);
  return;
}

// start the workday
void COptimizer::start(int threadCount)
{
  // for each company, do:
  for(auto & company : companyRegister)
  {
    printf("launch CTs\n");
    company.start();
  // create recieving thread
    //company.recievingThread = thread(&MyCompany::reciever, company);
  // create submitting thread
    //company.submittingThread = thread(&MyCompany::submitter, company);
  }
  // create first ProblemSolver
  printf("set first Solver\n");
  currentSolver = std::make_shared<MyProgtestSolver>(createProgtestSolver());
  // create "threadCount" worker thread
  // launch worker threads
  for(NoThreads = 0; NoThreads < threadCount; NoThreads++)
  {
    printf("launch WTs\n");
    workerThreads.push_back(thread(&COptimizer::worker, this));
  }
  // return to caller
  return;
}

// close the shop
void COptimizer::stop(void)
{
  printf("STOP\n");
  // wait for all threads to copmlete their tasks
  // termiante threads
  for(auto & company : companyRegister)
  {
    printf("wait for RT\n");
    company.recievingThread.join();
  }
  for(auto & workerT : workerThreads)
  {
    printf("wait for WT\n");
    workerT.join();
  }
  for(auto & company :companyRegister)
  {
    printf("wait for ST\n");
    company.submittingThread.join();
  }
  printf("END\n");
  // return to caller
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
