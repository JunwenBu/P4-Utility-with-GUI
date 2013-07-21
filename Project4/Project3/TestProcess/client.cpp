/////////////////////////////////////////////////////////////////////
// clent.cpp - child process spawned by TestClient.exe             //
//             run test same as the test in Project1 output result //
// ver 1.0  4/9/2012                                               //
// Language:    Visual C++, Visual Studio 2010, SP1                //
// Application: CSE 687 Project3                                   //
// Author:      Junwen Bu                                          //
/////////////////////////////////////////////////////////////////////
/*
 * Module Operations:
 * =================================
 * This module is a child process which will be spawned 
 * by TestClient.exe. It will load dll file and run test 
 * same as test in Project1, then it will display the test
 * result. Pass test result to Parent Process
 * 
 * Function Catalog:
 * =================================
 * int main(int argc, char *argv[]);
 *
 */

#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <conio.h>
#include "ITest.h"   // note that the client only needs the base class
                     // header file, not headers from derived classes
using namespace std;

// ----< main function >-----------------------
int main(int argc, char *argv[]) { 
  HANDLE console = CreateFile(L"CONOUT$",GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
  DWORD dwWritten;  // bytes written
  string logPath="", testname="default",conMsg = " \nTest load library:\n=============================================\n";
  if(argc > 1){
    try{ // output information about path and name of dll file
      conMsg+=(" \ndll file path and name: "+(string)argv[1]+"\n\n"); 
      testname=argv[2];
      logPath="../Project3/TestLogs/"+testname+"Log.txt";
      WriteFile(console, conMsg.c_str(), conMsg.size(), &dwWritten, NULL);
    }catch(std::exception& ex){
      conMsg+=ex.what();  // output cause of fail
      conMsg+=" \nFail! please check your path or dll conf in xml file ./n Test Fail! ...";
      WriteFile(console, conMsg.c_str(), conMsg.size(), &dwWritten, NULL);
      ofstream outfile(logPath,ofstream::app);
      outfile<<conMsg<<endl,outfile.close();
      cout<<testname+":Fail"<<endl; // output result
      return 1;}
  }else argv[1]="default dll path";
  string dllpath=argv[1];WCHAR wsz[128]; // convert char* to LPCWSTR
  swprintf(wsz,128,L"%S ",argv[1] );
  HMODULE hmod = LoadLibrary(wsz);
  if(!hmod){      // output information to show cause of fail
    std::string conMsg= "\n  failed to load dll file ... \n Test Fail! ...";
    WriteFile(console, conMsg.c_str(), conMsg.size(), &dwWritten, NULL);
    ofstream outfile(logPath,ofstream::app);
    outfile<<conMsg<<endl,outfile.close();
    cout<<testname+":Fail"<<endl; // output result
    return 1;}
  typedef ITest* (*gC)(); // typedef
  gC gCreate = (gC)GetProcAddress(hmod,"gCreate");
  if(!gCreate){
    conMsg= "\n  GetTestAddress failed for gCreate\n Test Fail! ...";
    WriteFile(console, conMsg.c_str(), conMsg.size(), &dwWritten, NULL);
    ofstream outfile(logPath,ofstream::app);
    outfile<<conMsg<<endl,outfile.close();
    cout<<testname+":Fail"<<endl; // output result
    return 1;}        // global factory function creates derived object 
  ITest* bptr = gCreate(); // create test object
  bptr->setName(testname); // set the name of test and log
  cout<<testname+":"+(bptr->test()?"Pass":"Fail")<<endl; // output result
  delete bptr;
  FreeLibrary(hmod);
  CloseHandle(console); // close console Handle 
  return 0;
}
// end main