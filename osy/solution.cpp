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
  u_int NoCompanies = 0;
  u_int NoThreads = 0;
  std::vector<MyCompany> companyRegister;
  std::vector<thread> workerThreads;
  static std::list<std::shared_ptr<MyProblemPack>> globalWaitingQueue;
  std::list<std::shared_ptr<MyProblemPack>>::iterator iteratorToUnprocessedPacks;
  std::shared_ptr<MyProgtestSolver> currentSolver;
  // ------------------------------------

  void worker();
};

// TODO: COptimizer implementation goes here
struct COptimizer::MyProblemPack
{
  MyProblemPack(AProblemPack problemPack, condition_variable * owner) : pack(problemPack), ownerCompany(owner), leftToProcess(problemPack->m_Problems.size()), leftToBeSolved(leftToProcess){}
  AProblemPack pack;
  // company tag
  condition_variable * ownerCompany;
  // left to process
  u_int leftToProcess;
  // left to be solved
  u_int leftToBeSolved;
};

struct COptimizer::MyProgtestSolver
{
  std::list<std::shared_ptr<MyProblemPack>>::iterator iteratorToProcessedPacks;
  AProgtestSolver progtestSolver;
};

struct COptimizer::MyCompany
{
  MyCompany(ACompany aCompany) : company(aCompany){} 
  ACompany company; 
  thread recievingThread;
  thread submittingThread;
  condition_variable CV_submit;
  std::deque<std::shared_ptr<MyProblemPack>> companyWaitingQueue;

  void reciever();
  void submitter();
};

// reciever thread main function
void COptimizer::MyCompany::reciever()
{
  // take new ProblemPacks and push_back them to the waiting queue
  // when new pack was recieved signalise worker threads
  // stop when nullptr was recieved = EOF
  AProblemPack newProblemPack;
  while(newProblemPack = company->waitForPack())
  {
    std::shared_ptr<MyProblemPack> newMyProblemPack = std::make_shared<MyProblemPack>(newProblemPack, &CV_submit);
    // may be critical
    companyWaitingQueue.push_back(newMyProblemPack);
    // probably is critical
    globalWaitingQueue.push_back(newMyProblemPack); // NEEDS STATIC !!!!!
    // wake workers up
    // leaves critical
  }
  // finish workers
  return;
}

// submitter thread main fucniton
void COptimizer::MyCompany::submitter()
{
  // check first ProblemPack in the queue if it is completed
  // if solved then submit it
  // otherwise sleep and wait for signal
  std::shared_ptr<MyProblemPack> firstPackInLine;
  while(true)
  {
    // may be critical
    firstPackInLine = companyWaitingQueue.front();
    if(firstPackInLine->leftToBeSolved == 0)
    {
      company->solvedPack(firstPackInLine->pack);
      // may be critical
      companyWaitingQueue.pop_front();
      // leaves critical
    }
    else
    {
      // leaves critical
      // sleep (semafor) - woken up by workers
    }
  }
  // finish when queue is empty
  return;
}

// worker thread main function
void COptimizer::worker()
{
  // check global waiting queue
  // put new problems to problem solver
  // if problem solver is full, launch problem solver
  // after problem solver finishes, mark problems as solved and wake dedicated submitter
  while(true)
  {
    // check global waiting queue
    std::shared_ptr<MyProblemPack> packToProcess = *iteratorToUnprocessedPacks;
    while(packToProcess == nullptr)
    {
      //sleep - woken up by reciever
      packToProcess = *iteratorToUnprocessedPacks;
    }

    if(packToProcess->leftToProcess > 0)
    {
      // add problem to ProgtestSolver
      packToProcess->leftToProcess--;
      currentSolver->progtestSolver->addProblem(packToProcess->pack->m_Problems.at(packToProcess->leftToProcess));
      
      // check if the solver is full
      if(!(currentSolver->progtestSolver->hasFreeCapacity()))
      {
        // create new solver for other workers
        std::shared_ptr<MyProgtestSolver> fullSolver = currentSolver;
        currentSolver = std::make_shared<MyProgtestSolver>(createProgtestSolver());
        // solve the problems
        size_t solved = fullSolver->progtestSolver->solve();
        // mark problems as solved
        while(solved > 0)
        {
          if(solved >= ((*(fullSolver->iteratorToProcessedPacks))->leftToBeSolved))
          {
            solved -= ((*(fullSolver->iteratorToProcessedPacks))->leftToBeSolved);
            ((*(fullSolver->iteratorToProcessedPacks))->leftToBeSolved) = 0;
            // wake dedicated company
            fullSolver->iteratorToProcessedPacks = globalWaitingQueue.erase(fullSolver->iteratorToProcessedPacks);
          }
          else
          {
            ((*(fullSolver->iteratorToProcessedPacks))->leftToBeSolved) -= solved;
            break; // solved = 0;
          }
        }
      }
    
    }
    // pack is being processed, move to next one
    else
    {
      ++iteratorToUnprocessedPacks;
    }
  }

  return;
}



// add new instance of company
void COptimizer::addCompany(ACompany company)
{
  // add new instance of company to data structure (vector?)
  companyRegister.emplace_back(company);
  return;
}

// start the workday
void COptimizer::start(int threadCount)
{
  // for each company, do:
  for(const auto & company : companyRegister)
  {
  // create recieving thread

  // create submitting thread
  // create "threadCount" worker thread
  // launch worker threads
  }
  // return to caller
  return;
}

// close the shop
void COptimizer::stop(void)
{
  // wait for all threads to copmlete their tasks
  // termiante threads
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
