#ifndef TESTPROCESS_H
#define TESTPROCESS_H
///////////////////////////////////////////////////////////////////////
// TestProcess.cpp - Create a Child Process to run test process      //
// ver 1.1  4/28/2012                                                //
// Language:    Visual C++, Visual Studio 2010, SP1                  //
// Application: CSE 687 Project3                                     //
// Author:      Junwen Bu                                            //
// Reference:   Jim Fawcett                                          //
///////////////////////////////////////////////////////////////////////
/*
 * Module Operations:
 * =================================
 * This module provide facilities to create an anonymous pipe to communicate
 * with a child process which runs the test process to do test by loading dll 
 * files produced from CSE687 Project#1, the child process will reads paraments 
 * passed by parent process,  The child process logs its results to standard 
 * output which is retrieved by its parent by using reads and write handles and
 * finally close the pipe.
 *
 * Required Files:
 * ===============
 * locks.h, locks.cpp, BlockingQueue.h, BlockingQueue.cpp
 *
 * Public Interface:
 * ==========================
 * string static getTime(char* testName, int startORend);
 * int createProcess(char* appname, char* argument="default argument");
 *
 * TestProcess p;
 * char* appName="../TestProtocol.exe";
 * return p.createProcess(appName);
 *
 * Private Function Catalog:
 * ==========================
 * char* appname, argument;
 * int declareStructures();          // declare structures, if fail to create pipe rerurn
 * string getLastErrorString();      // retrieve system error message
 * void setProcessParameters(appname,argument); // set parameters for CreatePipe
 */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include "BlockingQueue.h"
#include "locks.h"

using namespace std;
// class TestProcess
class TestProcess
{
public:
  // get local time 
  string static getTime(char* testName, int startORend);
  // create Child Process to do test by run appname with arguments in <char* argument>
  int createProcess(std::string testname, char* appname, BQueue<string>& receiveQueue, char* argument="default argument", int winPos=0);
private:
  // declare structures for CreateProcess
  STARTUPINFO startInfo;            // we fill to control startup
  PROCESS_INFORMATION procInfo;     // CreateProcess will fill
  HANDLE hRead, hWrite, hStdout;
  char *pCL, *aPPname;
  LPCTSTR appName;                  // program to run
  LPTSTR  commandLine;              // main's args
  LPSECURITY_ATTRIBUTES procSec;    // default security
  LPSECURITY_ATTRIBUTES thrdSec;    // default security
  BOOL inheritHandles;              // inherit so child can get pipe Write Handle
  DWORD createFlags;                // make a console
  LPVOID environment;               // caller's evironment
  LPCTSTR currentPath;              // default dir
  LPSTARTUPINFO pStartInfo;         // set process style
  LPPROCESS_INFORMATION pPrInfo;    // retrieve info
  SECURITY_ATTRIBUTES sa;
  int declareStructures();          // declare structures, if fail to create pipe rerurn
  string getLastErrorString();      // retrieve system error message
  void setProcessParameters(char* appname, char* argument, int i); // set parameters for CreatePipe
};
#endif