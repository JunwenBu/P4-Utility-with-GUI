#include "Display.h"
using namespace std;
using namespace SocketCommunicator;
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

// --< use sout and locker facilities in lock package to output massage >--
void Display::printmsg(const string& msg){
  sout<<locker<<msg<<unlocker;
}

// --< display back to menu information and send result to Test Client >--------------------------
void Display::displayResult(const string& resultStr){ 
  // string testid, author, name, and testdate are used to store 
  // id, author, name and date information of test
  string testid, author, name, testdate;
  int pos=resultStr.find(":");                       // find position of ':' in resultStr
  string testname=resultStr.substr(0,pos);           // name of test
  int idpos=testname.find("_-_");
  testid=testname.substr(0,idpos);                   // get test id
  int namepos=testname.find_last_of("_-_");
  author=testname.substr(idpos+3,namepos-idpos-5); // get test author
  name=testname.substr(namepos+1,testname.length()-namepos-1); // get test name
  testdate=TestProcess::getTime((char*)testname.c_str(),1);
  string passORfail=resultStr.substr(pos+1,4); // test result
  string result="\r\n -------------------< FORMAL RESULT >--------------------\r\n   Testname: <"
    + name+">"+"  ID: <"+testid+">   Author: <"+author +">\r\n   Datetime: "
    +testdate+"\r\n   Test Result:  <"+passORfail+"> \r\n";
  printmsg(result); // use printmsg function to print out
  try{
    sendResult(result);           // send Test result to Test Client
    sendInformalInfo(testname);   // send Informal Test result to Test Client
  }catch(...){/*nothing need to do, server will still work*/}
}

// --< connect to Client >--------------------------------
bool Display::connectClient(const string& _ip, int _port){
  try{ // set max try times to five
    isconnect=false;
    int tryCount=0, Maxtries=5;
    sndr = new SocketCommunicator::Sender();
    ip = _ip;      // set Test Client ip
    port = _port;  // set Test Client port
    while(++tryCount < Maxtries)
    { // try 'Maxtries' times to connect to the Client 
      if(sndr->connect(EndPoint(ip,port))){
        isconnect=true;
        break;
      }
      Sleep(3);
      cout << "\n  failed attempt to connect to endpoint(" 
        << _ip.c_str() << ", " << _port << ")"<<endl;
    }
    return isconnect;
  }catch(...){cout << "\n  failed to connect to endpoint(" 
    << _ip.c_str() << ", " << _port<< ")"<<endl;
  return false;
  }
}


// private functions:

// --< send test result msg to the Test Client >--------
void Display::sendResult(const string& resultSent){
  try
  { // set the body of the Message Object msgstr
    Message msgstr;
    if(isconnect){
      msgstr.body()=resultSent;
      sndr->PostMsg(msgstr);
    }
  }
  catch (...){cout << "\n  failed to connect to Test Client!"<<endl;}
}

// --< send informal test result to the Test Client >---------
void Display::sendInformalInfo(const string& testname){
  SocketCommunicator::Message msg;
  try{ 
    // get informal test result information from logger
    string fname="../Project3/TestLogs/"+testname+"Log.txt",lineStr="";
    vector<string> filelines=storeFile(fname);
    if(filelines.empty())
      return;
    // store informal informations from test logger line by line
    for(vector<string>::iterator iter=filelines.begin();iter!=filelines.end();iter++)
      lineStr+=*iter+"\r\n";
    lineStr+="\r\n";
    // set msg body and send out msg
    msg.body()=lineStr;  
    sndr->PostMsg(msg);
  }catch(...){
    cout<<endl<<"Send Informal Test Info Fail !"<<endl;
  }
}

// --< store files line by line into a vector<string> >---------------
vector<string> Display::storeFile(const string& fname){
  vector<string> lineVec;  // vector to store lines
  ifstream infile(fname); 
  string textline;         // string to store a single textline
  while (getline(infile,textline))
    lineVec.push_back(textline);
  return lineVec;
}


// ----< test stub >------------
#ifdef TEST_DISPLAY
int main()
{
  Display display;
  Display::printmsg("\nThis is a test\n");
  display.displayResult("testname:fail");
  // test connect
  bool isConnect=display.connectClient("loclahost","8888");
  cout<<isConnect<<endl;
  display.sendResult("This is a test");
}
#endif
// end test stub