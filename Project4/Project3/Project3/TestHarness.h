#ifndef TESTHARNESS_H
#define TESTHARNESS_H
///////////////////////////////////////////////////////////////////////
// TestHarness.h - core thread classes to create test process        //
// ver 1.2  4/28/2012                                                //
// Language:    Visual C++, Visual Studio 2010, SP1                  //
// Application: CSE 687 Project3                                     //
// Author:      Junwen Bu                                            //
///////////////////////////////////////////////////////////////////////
/*
 * Module Operations:
 * =================================
 * This module support core thread classes used in Test Harness.
 * class doCreate : thread to create Test Process
 * class ProcessCreator : thread to receive input conf info from queue
 * class ResultReceiver : thread to receive test result and sent result 
 *
 * Required Files:
 * ===============
 * locks.h, locks.cpp, BlockingQueue.h, BlockingQueue.cpp,
 * threads.h, thread.cpp, TestProcess.h, TestProcess.cpp,
 * Display.h, Display.cpp.
 *
 * Public Interface:
 * ==========================
 * run functions of classes:
 * run();      // run function of thread classes
 * ---------------------------------------------
 * constructor of classes:
 * class doCreate : doCreate(BQueue<std::string>& rQ, string dll, string testExec,int wPos);
 * class ProcessCreator : ProcessCreator(BQueue<std::string>& Q, BQueue<std::string>& rQ);
 * class ResultReceiver : ResultReceiver(BQueue<std::string>& Q);
 * -------------------------------------------------------------
 * HOW TO USE:
 * =========================
 * ClassObject cO;
 * thread th(cO);
 * cO.start();
 * cO.wait(); ...
 *
 * Private Function Catalog:
 * ==========================
 * NULL
 *
 * History
 * =========================
 * 4/28/2012
 * Do a few modifies to satisfy
 * OOD Project 4
 */

#include "blockingQueue.h"
#include "threads.h"
#include "locks.h"
#include "TestProcess.h"
#include "Display.h"
#include <string>
#include <iostream>
#include <sstream>
#include <ctime>

///////////////////////////////////////////////////////
// class doCreate : thread to create Test Process 
//-------------------------------------------------------
// spawn a separate test process that carries out all of 
// the processing specified in its test configuration
//////////////////////////////////////////////////////
class doCreate : public Thread_Processing<doCreate>
{
public:
  // constructor of doCreate
  doCreate(BQueue<std::string>& rQ, string dll, string testExec,int wPos);
  void run();               // run function of class doCreate
private:
  BQueue<std::string>& _rQ; // blocking Queue: to receiver test result will used by Process
  string _dll;              // used to store dll path and name info
  string _testExec;         // the name of the executable file 
  int _windowPos;           // used to modify the position of console window
};

/////////////////////////////////////////////////////////////////////
// class ProcessCreator : thread to receive input conf info from queue
//--------------------------------------------------------------------
// deQueue dll info from input Queue and then create doCreate thread
/////////////////////////////////////////////////////////////////////
class ProcessCreator : public Thread_Processing<ProcessCreator>
{
public:
  // constructor of ProcessCreator
  ProcessCreator(BQueue<std::string>& Q, BQueue<std::string>& rQ);
  void run(); // run function of calss ProcessCreator
private:
  BQueue<std::string>& _Q;     // blocking queue to store input data
  BQueue<std::string>& _rQ;    // blocking queue to store test result
  vector<HANDLE> handleVec;    // vector to store thread handles;
  int _winPos;                 // used to modify the position of console window
};

/////////////////////////////////////////////////////////////////////
// class ResultReceiver : thread to receive test result 
//--------------------------------------------------------------------
// thread class to recieve test result and then store result into
// resultQueue: a blocking queue and call displayResult function 
// in Display package to display result info in the main console
/////////////////////////////////////////////////////////////////////
class ResultReceiver : public Thread_Processing<ProcessCreator>
{
public:
  // constructor of ResultReceiver
  ResultReceiver(BQueue<std::string>& Q);
  void run();              // run function of ResultReceiver
private:
  BQueue<std::string>& _Q; // blocking queue to store test result
};

#endif