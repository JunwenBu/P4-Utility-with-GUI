///////////////////////////////////////////////////////////////////////
// TestHarness.cpp - core thread classes to create test process      //
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
* class ResultReceiver : thread to receive test result and send result
*
* provide test stub
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

#include "TestHarness.h"

///////////////////////////////////////////////////////
// class doCreate : thread to create Test Process 
//-------------------------------------------------------
// spawn a separate test process that carries out all of 
// the processing specified in its test configuration
//////////////////////////////////////////////////////

// --< constructor of doCreate >----------------------------
doCreate::doCreate(BQueue<std::string>& rQ, string dll, 
	string testExec,int wPos):_rQ(rQ),_dll(dll),_testExec(testExec),_windowPos(wPos){}

// --< run function >---------------------------------------
void doCreate::run()
{
	int position = _dll.find(".dll");
	string dllpath = _dll.substr(0,position+4);
	// Test Server display dll enqueue msg
	Display::printmsg("\n  enQueue "+dllpath+" ---> Create Test Process\n");
	int pos=_dll.find(".dll "); 
	// get name of dll
	string name = _dll.substr(pos+5, _dll.size()-pos-5); 
	_dll="one "+_dll;  
	TestProcess testP; 
	// create test process
	testP.createProcess(name,(char*)_testExec.c_str(),
		_rQ, (char*)_dll.c_str(),_windowPos);
}

/////////////////////////////////////////////////////////////////////
// class ProcessCreator : thread to receive input conf info from queue
//--------------------------------------------------------------------
// deQueue dll info from input Queue and then create doCreate thread
/////////////////////////////////////////////////////////////////////

// --< constructor of ProcessCreator >---------------------------------
ProcessCreator::ProcessCreator(BQueue<std::string>& Q, BQueue<std::string>& rQ) 
	: _Q(Q),_rQ(rQ), _winPos(0){}

// --< run function >---------------------------------------
void ProcessCreator::run()
{
	std::string dll="", testExec = "../Debug/TestProcess.exe";
	while(dll != "quit"){
		dll = _Q.deQ();
		if(dll.compare("quit")==0)
			break;
		// modify console window position
		_winPos%=10;   
		_winPos++;
		doCreate dC(_rQ, dll,testExec,_winPos);
		tthread* t=new tthread(dC);   // create doCreate thread 
		HANDLE tHandle=t->handle();  // get thread handle
		handleVec.push_back(tHandle);// store handle in handleVec
		t->start();                   // start thread
	}
	for(vector<HANDLE>::iterator iter=handleVec.begin(); iter!=handleVec.end(); ++iter)
		WaitForSingleObject(*iter,INFINITE); // wait
}

/////////////////////////////////////////////////////////////////////
// class ResultReceiver : thread to receive test result 
//--------------------------------------------------------------------
// thread class to receive test result and then store result into
// resultQueue: a blocking queue and call displayResult function 
// in Display package to display result info in the main console
/////////////////////////////////////////////////////////////////////

// --< constructor of ResultReceiver >--------------------------
ResultReceiver::ResultReceiver(BQueue<std::string>& Q) : _Q(Q){}

// --< run function >---------------------------------------
void ResultReceiver::run()
{
	// create Display object
	Display display;
	std::string resultStr;
	while(resultStr != "quit")
	{ // get test result from a blocking queue: ReceiveQueue
		resultStr=_Q.deQ();
		// set server sender ip and port
		// ip is "localhost", and port is 1354
		display.connectClient("localhost",1354);
		if(resultStr.compare("quit")==0) break;
		// call displayResult Function to display result
		// this function also send result to Test Client
		display.displayResult(resultStr); 
	}
}


// ---- <test stub> --------------------------------------------
#ifdef TEST_TESTHARNESS
void main(int argc, char *argv[]) {
	cout << "\n  Run Test Harness "<< 
		"\n =================================================\n";
	// blockingqueue: inputQueue, blockingqueue: resultQueue
	BQueue<string> inputQueue, resultQueue; 
	std::string dllpath;     // the path of dll file
	ProcessCreator pCreator(inputQueue, resultQueue);
	ResultReceiver rReceiver(resultQueue);
	thread th(pCreator);    // create ProcessCreator Thread
	thread rth(rReceiver);  // create ReceiverCreator Thread
	th.start();              // start ProcessCreator Thread
	rth.start();             // start ProcessCreator Thread
	string choice="c";     
	while(true){
		if(choice.compare("c")==0){ // if choice is "c", run TestHarness
			rPth->start();
			for(int i=0; i<2; i++){
				dllpath="../TestDlls/test.dll";
				inputQueue.enQ(dllpath);
				string outmsg="\n  enQueue: "+dllpath+"\n";
				sout << locker <<  outmsg << unlocker;
			}
			rPth->wait();
		}else if(choice.compare("q")==0){ // if choice is "q", quit TestHarness
			while(inputQueue.size()!=0||resultQueue.size()!=0){}
			inputQueue.enQ("quit");
			resultQueue.enQ("quit");
			break;
		}
		if(inputQueue.size()==0&&resultQueue.size()==0)
			getline(cin,choice);
	}
	th.wait();
	rth.wait();
}
#endif
// end test stub