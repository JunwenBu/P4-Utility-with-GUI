#ifndef XMLWRITER_H
#define XMLWRITER_H
///////////////////////////////////////////////////////////////////////
// XmlWriter.h - Create XML Strings                                  //
// ver 2                                                             //
// Language:    Visual C++, Visual Studio 2010, SP1                  //
// platform:    Toshiba Portege R705, Windows 7, Home Premium, SP1   //
// Application: Summer 2011 Projects                                 //
// Author:      Jim Fawcett, Syracuse University, CST 4-187          //
//              www.lcs.syr.edu/faculty/fawcett, (315) 443-3948      //
///////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * ===================
 * This package contains a single class, XmlWriter.  It provides a
 * simple interface to build simple XML strings.  The package does
 * not support editing XML strings.  For that, you should use the
 * XmlDocument package.
 *
 * XmlReader and XmlWriter supercede both XmlTran and XmlParser.
 *
 * Required Files:
 * ===============
 * XmlWriter.h, XmlWriter.cpp
 *
 * Build Process:
 * ==============
 * cl /Eha XmlWriter.cpp
 *
 * Maintenance History:
 * ====================
 * ver 2 : 27 Jun 11
 * - added verbose output to illustrate how XmlWriter works
 * ver 1 : 26 Jun 11
 * - first release
 */
#include <string>

class XmlWriter
{
public:
  typedef std::pair<std::string,std::string> attribElem;
  void start(const std::string& tag);
  void addBody(const std::string& body);
  void addAttribute(const std::string& name, const std::string& value);
  void end();
  void addDeclaration();
  void addComment(const std::string& comment);
  std::string xml();
private:
  std::string _tag;
  std::string _xml;
  size_t position;
};

inline std::string XmlWriter::xml() { return _xml; }

#endif
