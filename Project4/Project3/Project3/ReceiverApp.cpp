/////////////////////////////////////////////////////////////////////
// ReceiverApp.cpp - Demonstrates simple use of SocketCommunicator //
//                     Receives messages from SenderApp            //
// ver 2.1                                                         //
// Language:      Visual C++, 2008                                 //
// Platform:      Dell Dimension T7400, Win 7 Pro                  //
// Application:   Utility for CSE687 projects, Spring 2010         //
// Author:        Jim Fawcett, Syracuse University, CST 4-187      //
//                (315) 443-3948, jfawcett@twcny.rr.com            //
/////////////////////////////////////////////////////////////////////
/*
  Note:
  - This end starts receiving then when done starts sending.
  - Since we are running Sender and Receiver on same thread, we must 
    first do all our receiving then do all our sending.
  - There would be no such constraint if we ran sender and receiver on
    separate threads.
  - That is what you should do in Project #4.

  Maintenance History:
  ====================
  ver 2.1 : 24 Apr 11
  - Added namespace SocketCommunicator to Comm.h and Message.h which
    will allow you to avoid conflicts with the .Net Message type.
    That means you will need to declare "using namespace SocketCommunicator;"
    in some places in your code.
  - Fixed a bug discovered by Jingyi Ren in the Message type handling.
    She provided a solution, and that worked and has been incorporated.
  ver 2.0 : 23 Apr 11
  - Thanks to Amit Ahlawat, Himanshu Gupta, Kajal Kapoor, and Jingyi Ren
    for submitting bug reports.
  - added base64 encoding of message bodies when sending files to
    avoid problems with XML markup characters in the file's text.
  - you may wish to encode all messages to avoid switching back and
    forth (as I did below) when sending files.
  ver 1.1 : 17 Apr 11
  - added a Sender section to demonstrate two-way communication
  ver 1.0 : 16 Apr 10
  - first release
*/

#include "comm.h"
#include "Base64.h"
#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>

using namespace SocketCommunicator;

void main()
{
  std::cout << "\n  Starting ReceiverApp";
  std::cout << "\n ======================\n";

  /////////////////////////////////////////////////////////////
  // If another listener is using the port you specify, below,
  // the rcvr constructor will throw an exception, naming a
  // "bind error".

  try
  {
    Receiver rcvr(8088);
    Message msg, vmsg;
    boolean isfile = false;
    std::string path="",temptext;
    while(true)
    {
      msg=rcvr.GetMsg();
      if(msg.body()!="file"&&!isfile){
        std::cout << "\n  Getting msg: " << msg.ToString()<<std::endl
          <<"============================================"<<std::endl;
        continue;
      }
      temptext="";
      while((msg.body()!="file")&&isfile)
      {
        std::vector<char> vdecoded = Base64::decode(msg.body());
        std::string decodedMsg(vdecoded.begin(), vdecoded.end());
        //std::cout << "\n  Getting file content : " << decodedMsg;
        temptext+=decodedMsg;

        msg=rcvr.GetMsg();
        if(msg.body()=="end"){
          std::ofstream outfile(path,std::ios::binary);
          temptext.erase(temptext.length()-1,1);
          outfile<<temptext;
          outfile.close();
          outfile.clear();
          isfile=false;
        }
      }
      if(msg.body()=="file"){
        isfile=true;
        msg=rcvr.GetMsg();
        std::cout<<std::endl<<"File Name: "<<msg.body().c_str()<<std::endl;
        path = "../Test/"+msg.body();
        std::ofstream fout(path,std::ios::binary);
        fout.close();
        fout.clear();
      }
      if(msg.body()=="quit")
        break;
    }

    std::cout << "\n\n  shutting down ReceiverApp Receiver";

    ///////////////////////////////////////////////////////////
    // The following lines prompt for shutdown:
       //std::cout << "\n  press key to exit: ";
       //std::cout.flush();
       //_getche();  // from conio.h
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what();
  }
  std::cout << "\n\n";
}

