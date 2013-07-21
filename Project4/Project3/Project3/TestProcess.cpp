#ifndef PROCESS_H
#define PROCESS_H
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
 * provide test stub
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
#include "TestProcess.h"

// --< get local time >---------------------------------
string TestProcess::getTime(char* testName, int startORend)
{
  struct tm newtime;
  __time32_t aclock;
  char buffer[32];
  errno_t errNum;
  _time32( &aclock ); // Get time in seconds.
  _localtime32_s( &newtime, &aclock );      // Convert time to struct tm form.
  errNum = asctime_s(buffer, 32, &newtime); // Print local time as a string.
  if(errNum){
    printf("Error code: %d", (int)errNum);
    return "";
  }
  if(startORend==0)
    printf( "\n      %s STARTs at time: %s",testName, buffer );
  return buffer;
}

// --< create Child Process to do test by run appname with arguments in <char* argument> >--------------
int TestProcess::createProcess(string testname, char* appname,BQueue<string>& receiveQueue, char* argument, int winPos){ 
  // string testid, author and name are used to store id, author and name info of the test file 
  if(declareStructures())return 1;
  string check=appname, testid, author, name;
  int idpos=testname.find("_-_");
  int namepos=testname.find_last_of("_-_");
  testid=testname.substr(0,idpos);
  author=testname.substr(idpos+3,namepos-idpos-5);
  name=testname.substr(namepos+1,testname.length()-namepos-1);
  if(check.compare("quit")==0) return 1;
  setProcessParameters(appname, argument, winPos);
  string logPath="../Project3/TestLogs/"+testname+"Log.txt";
  ofstream outfile(logPath,ofstream::out);
  outfile<<" <Informal Test Result>\n TestName: "<<name<<"  ID: "
    <<testid<<"  by: "<<author<<"\n Test Logger Start:"<<endl; 
  BOOL OK = CreateProcess(appName,commandLine,procSec,thrdSec,inheritHandles,
    createFlags,environment,currentPath,pStartInfo,pPrInfo);
  CloseHandle(hWrite);  // closes parent's not child's handle
  SetStdHandle((DWORD)STD_OUTPUT_HANDLE,hStdout);  // restore parent stdout handle
  if(!OK) cout<<"\nFailed to create process: "<<appname<<"\n  "<<getLastErrorString().c_str()<<endl;
  // read child's output from pipe
  const size_t BUFSIZE = 256;
  char buffer[BUFSIZE+1];
  DWORD dwRead;
  for(;;){ // ReadFile will block until child writes to pipe.
    if(!ReadFile(hRead,buffer,BUFSIZE,&dwRead,NULL))break;
    buffer[dwRead] = 0;
    std::string str(buffer);
    if(int pos = str.find(":")!=str.npos)
      receiveQueue.enQ(str);
    if(str.find("quit") < str.size()) break;
  }
  WaitForSingleObject(procInfo.hProcess,INFINITE);
  CloseHandle(hRead); // close hRead handle
  return 0;
}

//----< retrieve system error message >--------------------------------
string TestProcess::getLastErrorString()
{ // almost wholely reference Jim Fawcett's code
  DWORD errorCode = GetLastError();
  DWORD dwFlags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER;
  LPCVOID lpSource = NULL;
  DWORD dwMessageID = errorCode;
  DWORD dwLanguageId = MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US);
  LPSTR lpBuffer;
  DWORD nSize = 0;
  va_list *Arguments = NULL;
  FormatMessage(dwFlags,lpSource,dwMessageID,dwLanguageId, 
    (LPTSTR)&lpBuffer,nSize,Arguments);
  if(lpBuffer)return std::string(lpBuffer);
  return "";
}

// --< declare structures, if fail to create pipe rerurn >--------------------
int TestProcess::declareStructures()
{
  sa.bInheritHandle = TRUE;         // tells pipe to support inheritance of handles
  sa.lpSecurityDescriptor = NULL;   // client process security properties
  sa.nLength = sizeof(sa);
  if(!::CreatePipe(&hRead, &hWrite, &sa, 0)){
    std::cout << "\n  could not create anonymous pipe\n\n"<< getLastErrorString().c_str() << endl;
    return 1;
  }
  return 0;
}

// --< set parameters for CreatePipe >----------------------------------
void TestProcess::setProcessParameters(char* appname, char* argument, int i)
{
  hStdout = GetStdHandle(STD_OUTPUT_HANDLE);   // save parent stdout handle
  // Set parent stdout handle to pipe write handle which lets child access pipe.
  SetStdHandle(STD_OUTPUT_HANDLE,hWrite);
  aPPname=appname;              // create child's command line arguments
  pCL =argument;                // set parameters for CreateProcess
  appName                = aPPname;            // program to run
  commandLine            = pCL;                // main's arguments
  procSec                = &sa;                // default security
  thrdSec                = NULL;               // default security
  inheritHandles         = TRUE;               // inherit so child can get pipe Write Handle
  createFlags            = CREATE_NEW_CONSOLE; // make a console
  environment            = NULL;               // caller's environment
  currentPath            = NULL;               // default directory
  pStartInfo             = &startInfo;         // set process style
  pPrInfo                = &procInfo;          // retrieve info
  GetStartupInfo(&startInfo);   // set STARTUPINFO with the parameters we just defined
  startInfo.lpTitle=appname;
  // STARTF_USESTDHANDLES tells new process to use parent's standard I/O handles
  startInfo.dwFlags = STARTF_USEPOSITION | STARTF_USESIZE | STARTF_USESTDHANDLES;
  startInfo.dwX     = 30*i+130;
  startInfo.dwY     = 20*i+30;
  startInfo.dwXSize = 500;
  startInfo.dwYSize = 500;
  startInfo.hStdOutput = hWrite;
  startInfo.hStdError = hWrite;
}
#endif

//#define TEST_PROCESS
//----< test stub >--------------
#ifdef TEST_PROCESS
int main(int argc, char *argv[]) {
  TestProcess p; // Create TestProcess object
  char* appName="D:/Visual Studio WorkSpace/TestProtocol/Debug/TestProtocol.exe";
  return p.createProcess(appName);
}
#endif
// end test stub