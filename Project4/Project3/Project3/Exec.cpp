///////////////////////////////////////////////////////////////////////
// Exec.cpp - run TestHarness                                        //
// ver 1.1 4/29/2012                                                  //
// Language:    Visual C++, Visual Studio 2010, SP1                  //
// Application: CSE 687 Project3                                     //
// Author:      Junwen Bu                                            //
///////////////////////////////////////////////////////////////////////
/*
* Module Operations:
* =================================================
* Old version:
* This is the executive package, it do stuffs as follows:
* gets test dlls info from an xml file,retrieves test configuration 
* message from the input queue,spawns threads to process that 
* configuration and goes back to retrieve another enqueued test
* configuration.Each thread spawns a test process and passes it the 
* test configuration, then blocks waiting for the test results.  
* When its test process exits it retrieves the results and posts 
* them to a test harness results queue, and presents the results 
* of each test to the user.
* 
* New version
* The newer version get xml configuration from a Test Client by 
* using utility of Socket Communicator.
*
* Required Files:
* ==========================================
* blockingQueue.h, blockingQueue.cpp, 
* threads.h, threads.cpp, XmlReader.h, XmlReader.cpp
* locks.h, locks.cpp, TestHarness.h, Testharness.cpp
* Display.h, Display.cpp
* 
* Function(Interface) Catalog:
* ==========================================
* // read xml file to get dll name, test id, test author
* vector<string> getXmlInfo(string& xmlStr); 
* main(argc, argv[]); // the main function 
*/
#include <iostream>
#include "Comm.h"
#include <fstream>
#include <string>
#include <conio.h>
#include "Base64.h"
#include "blockingQueue.h"
#include "threads.h"
#include "locks.h"
#include "XmlReader.h"
#include "TestHarness.h"
#include "TestProcess.h"
#include "Display.h"
#include <ctime>
using namespace std;
using namespace SocketCommunicator;

// ----< read xml file to get dll name, test id, test author >------------
vector<string> getXmlInfo(string& xmlStr)
{
	vector<string> retVec; // vector to return
	string testid, dllname, testauthor;
	XmlReader rdr(xmlStr); // create XmlReader object rdr
	while(rdr.next()){
		if(rdr.tag().compare("id")==0)
			testid=""+rdr.body();      // get test id
		if(rdr.tag().compare("dll")==0)
			dllname=""+rdr.body();     // get test name
		if(rdr.tag().compare("author")==0){
			testauthor=""+rdr.body(); // get test author
			string tempstr="../Project3/TestDlls/"+dllname+" "+testid+"_-_"+testauthor+"_-_"+dllname;
			tempstr.erase(tempstr.length()-4,4);
			retVec.push_back(tempstr);
		}
	}
	return retVec;
}

// ----< main function >----------------------------------------
void main(int argc, char *argv[]) { /* initialization: */
	cout << "\n  Test Harness Server "<< 
		"\n =================================================\n";
	Receiver rcvr(8088); 
	Message msg, vmsg; 
	boolean isdllfile = false; 
	string path="",temptext="";
	BQueue<string> inputQueue, resultQueue; // input blocking queue and result blocking queue
	ProcessCreator pCreator(inputQueue, resultQueue); 
	ResultReceiver rReceiver(resultQueue);
	thread th(pCreator),rth(rReceiver);th.start(),rth.start(); // thread operations: create and start
	while(true){  // Server always run ...
		msg=rcvr.GetMsg();
		if(msg.body()=="xmlfile"&&!isdllfile){
			msg=rcvr.GetMsg();     // this msg will store XML name info
			std::string xmlname=msg.body().c_str();
			std::cout<<std::endl<<"Xml File Name: "<<xmlname<<std::endl;
			bool isXmlFile=true;msg=rcvr.GetMsg(),temptext="";
			while(isXmlFile){     // if msg still belongs to XML file than decode
				vector<char> vdecoded = Base64::decode(msg.body());
				string decodedMsg(vdecoded.begin(), vdecoded.end());
				temptext+=decodedMsg;
				msg=rcvr.GetMsg();
				if(msg.body()=="end"){
					isXmlFile=false;  // set isXmlFile to false
					temptext.erase(temptext.length()-1,1);
					vector<string> infoVec=getXmlInfo(temptext);
					if(infoVec.empty())cout<<xmlname<<" is empty!"<<endl;
					else for(vector<string>::iterator iter=infoVec.begin();iter!=infoVec.end();++iter)
						inputQueue.enQ(*iter);
				} // end inner if
			} //end while
			continue;
		} // endl outer if

		temptext="";
		while((msg.body()!="dllfile")&&isdllfile){
			std::vector<char> vdecoded = Base64::decode(msg.body());
			std::string decodedMsg(vdecoded.begin(), vdecoded.end());
			temptext+=decodedMsg;msg=rcvr.GetMsg();
			if(msg.body()=="end"){ // store content into file
				std::ofstream outfile(path,std::ios::binary);
				temptext.erase(temptext.length()-1,1);
				outfile<<temptext;outfile.close();
				isdllfile=false;
			}/*end if*/ 
		} /* end while */
		if(msg.body()=="dllfile"){
			isdllfile=true;    // set isdllfile to true
			msg=rcvr.GetMsg();
			std::cout<<std::endl<<"  Get dll file: "<<msg.body().c_str()<<std::endl;
			path = "../Project3/TestDlls/"+msg.body();
			std::ofstream fout(path,std::ios::binary); // create ofstream
			fout.close();
		} /* end if */
	}
	th.wait(),rth.wait();
}

// end main
