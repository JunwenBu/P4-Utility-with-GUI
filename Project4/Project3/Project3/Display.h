#ifndef DISPLAY_H
#define DISPLAY_H
///////////////////////////////////////////////////////////////////////
// Display.h - This class support operations to return info to user  //
// ver 1.1 4/29/2012                                                 //
// Language:    Visual C++, Visual Studio 2010, SP1                  //
// Application: CSE 687 Project3                                     //
// Author:      Junwen Bu                                            //
///////////////////////////////////////////////////////////////////////
/*
 * Module Operations:
 * =================================
 * This module provide user with functions used to display information in
 * Project3:TestHarness, it includes three functions: printmsg - using sout
 * and locker to display msgs; backtoMenu - display back to menu info in 
 * console; displayResult - display test result in TestHarness console.
 *
 * In newer version, the module provide functions to send test result
 * back to Test Client, the test result are formal result which include 
 * test id, Test dateTime, Test author information and informal result
 * including almost ever test infor designed by Tester.
 *
 * Required Files:
 * ===============
 * locks.h, locks.cpp, TestHarness.h, TestHarness.cpp
 * Base64.h, Base64.cpp, Comm.h, Message.h, 
 * Communicator.lib
 *
 * Public Interface:
 * ==========================
 * void static printmsg(const string& msg);     // use sout and locker facilities in lock package to output massage
 * void displayResult(const string& resultStr); // display result of tests to Project3:TestHarness console, 
 *                                              // send result to Client              
 * Display display; std::string resultStr, msg; ... 
 * display.dispalyResult(resultStr);
 * Display::printmsg(msg);
 *
 * Private Function Catalog:
 * ==========================
 * vector<string> storeFile(const string& fname); // store files line by line into a vector<string>
 * void sendResult(const string& resultSent);     // send test result to the Test Client
 * void sendInformalInfo(const string& testname); // send informal test result to the Test Client
 *
 * History
 *===========================
 * ver 1.1 4/29/2012 
 *    add Server Sender Facilities
 * ver 1.0 4/9/2012
 */
#include "locks.h"
#include "Base64.h"
#include "TestHarness.h"
#include "Comm.h"
#include "Message.h"
#include <iostream>
#include <string>
using namespace std;
using namespace SocketCommunicator;
// class Display:
class Display{
public:
  void static printmsg(const string& msg);     // use sout and locker facilities in lock package to output massage
  void displayResult(const string& resultStr); // display result of tests to Project3:TestHarness console
                                                 // and send result to Client
  bool connectClient(const string& _ip, int _port); // connect to Client
  ~Display(){delete sndr;}
private:
  vector<string> storeFile(const string& fname); // store files line by line into a vector<string>
  void sendResult(const string& resultSent);     // send test result msg to the Test Client
  void sendInformalInfo(const string& testname); // send informal test result to the Test Client
  Sender *sndr; // SocketCommunicator::Sender Object                                   
  string ip;    // ip  of Test Client
  int port;     // port of Test Client
  bool isconnect;
};
#endif