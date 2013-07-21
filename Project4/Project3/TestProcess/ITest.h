#ifndef ITEST_H
#define ITEST_H
/////////////////////////////////////////////////////////////////////////////
// ITest.h - Interface for Test Driver				                             //																                                   //
// ver 1.1                                                                 //
// Language:    Visual C++ 2010                                            //
// Application: Project #1, Spring 2012                                    //
// Author:      Junwen Bu                                                  //
// Reference:   Jim Fawcett                                                //
/////////////////////////////////////////////////////////////////////////////
/*
 * Module Operations:
 * =================================
 * ITest interface consisting of the methods static ITest* create() 
 * and bool test(). The create function is a factory method that creates 
 * instances of whatever classes are needed to execute the test method.  
 * Function setName(string) set the name of Test which is a tag of the test
 *
 * Required Files:
 * =================================
 * depends on the packages you test or include those 
 * packages in client package
 *
 * Interface:
 * ==========================
 * static ITest* create();              // factory methods to do create
 * virtual bool test()=0;               // do test
 * virtual void setName(std::string)=0; // set name of test *
 *
 * Private Function Catalog:
 * ==========================
 * NULL
 */

#ifdef IN_DLL
#define DLL_DECL __declspec(dllexport)
#else
#define DLL_DECL __declspec(dllimport)
#endif

typedef std::string stdStr;
class DLL_DECL ITest
{
public:
  // virtual destructor
  virtual ~ITest(void) {}  
  /* factory method that creates instances of whatever
     classes are needed to execute the test method */
  static ITest* create();  
  virtual bool test()=0;               // do test
  virtual void setName(std::string)=0; // set name of test
};

extern "C" { DLL_DECL ITest* gCreate(); }  // global object factory
#endif