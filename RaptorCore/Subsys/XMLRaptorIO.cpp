// XMLRaptorIO.cpp: implementation of the CXMLRaptorIO class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_XMLRAPTORIO_H__DD049FDC_A319_4028_9759_E858401EB55E__INCLUDED_)
    #include "XMLRaptorIO.h"
#endif

#ifndef __CGLTYPES_HPP__
    #include "System/CGLTypes.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif

#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>

using xercesc::MemBufInputSource;
using xercesc::LocalFileInputSource;
using xercesc::XercesDOMParser;
using xercesc::XMLException;
using xercesc::DOMException;
using xercesc::DOMNode;
using xercesc::DOMNamedNodeMap;
using xercesc::XMLString;
using xercesc::DOMNodeList;
using xercesc::XMLPlatformUtils;
using xercesc::SAXParseException;
using xercesc::ErrorHandler;
using xercesc::AbstractDOMParser;

///////////////////////////////////////////////////////////////////////////////////////////////////
//  Static and constants
//
RAPTOR_NAMESPACE_BEGIN

class CRaptorXMLErrorHandler : public ErrorHandler
{
public:
    CRaptorXMLErrorHandler() {}
    virtual ~CRaptorXMLErrorHandler() {};

    virtual void warning(const SAXParseException& exc)
    {
        stringstream s;
        s << XMLString::transcode(exc.getMessage());
        s << " line: " << exc.getLineNumber();
        s << " pos: " << exc.getColumnNumber();
		RAPTOR_WARNING(CPersistence::CPersistenceClassID::GetClassId(),s.str());
    }
    virtual void error(const SAXParseException& exc)
    {
        stringstream s;
        s << XMLString::transcode(exc.getMessage());
        s << " line: " << exc.getLineNumber();
        s << " pos: " << exc.getColumnNumber();
		RAPTOR_ERROR(CPersistence::CPersistenceClassID::GetClassId(),s.str());
    }
    virtual void fatalError(const SAXParseException& exc)
    {
        stringstream s;
        s << XMLString::transcode(exc.getMessage());
        s << " line: " << exc.getLineNumber();
        s << " pos: " << exc.getColumnNumber();
		RAPTOR_FATAL(CPersistence::CPersistenceClassID::GetClassId(),s.str());
    }
    virtual void resetErrors() {};
};

static CRaptorXMLErrorHandler raptorXMLErrorHandler;

unsigned int CXMLRaptorIO::terminateCount = 0;

RAPTOR_NAMESPACE_END
//
///////////////////////////////////////////////////////////////////////////////////////////////////


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXMLRaptorIO::CXMLRaptorIO(const std::string& streamName, CRaptorIO::IO_KIND kind)
	:CRaptorIO(streamName,kind),
	xml_parser(NULL),nodePos(-1),currentNode(NULL),parentNode(NULL)
{
    try
    {
        XMLPlatformUtils::Initialize();
        terminateCount++;

        if (xml_parser == NULL)
        {
            xml_parser = new XercesDOMParser;
            xml_parser->setErrorHandler(&raptorXMLErrorHandler);
            xml_parser->setValidationScheme(AbstractDOMParser::Val_Always);
            xml_parser->setDoNamespaces(true);
            xml_parser->setDoSchema(true);
            xml_parser->setValidationSchemaFullChecking(true);
        }
    }
    catch(const XMLException &toCatch)
    {
        stringstream s;
		s << "Error during Xerces-c Initialization. Exception message:";
        s << XMLString::transcode(toCatch.getMessage());
        RAPTOR_ERROR(CPersistence::CPersistenceClassID::GetClassId(), s.str());
    }
}

CXMLRaptorIO::~CXMLRaptorIO()
{
    terminateCount--;

    if (xml_parser != NULL)
        delete xml_parser;

    if (terminateCount == 0)
        XMLPlatformUtils::Terminate();
}


CRaptorIO& CXMLRaptorIO::operator<<(char i)  { return *this; }
CRaptorIO& CXMLRaptorIO::operator>>(char& i)  { return *this; }

CRaptorIO& CXMLRaptorIO::operator<<(unsigned short i)  { return *this; }
CRaptorIO& CXMLRaptorIO::operator>>(unsigned short& i)  { return *this; }

CRaptorIO& CXMLRaptorIO::operator<<(bool i)  { return *this; }
CRaptorIO& CXMLRaptorIO::operator>>(bool& i)
{
	if (currentNode != NULL)
	{
		char *str = XMLString::transcode(currentNode->getNodeValue());

		if (str != NULL)
		{
			char *c = str;
			while (0 != *c)
				*c = toupper(*c++);

			if (!strcmp(str,"TRUE") ||
				!strcmp(str,"YES"))
				i = true;
			else if (!strcmp(str,"FALSE") ||
					!strcmp(str,"NO"))
				i = false;
			else
				i = (atoi(str) != 0);

			XMLString::release(&str);
		}
	}

    getNextValue();

    return *this; 
}

CRaptorIO& CXMLRaptorIO::operator<<(int i)  { return *this; }
CRaptorIO& CXMLRaptorIO::operator>>(int& i)  
{
	if (currentNode != NULL)
	{
		char *str = XMLString::transcode(currentNode->getNodeValue());

		if (str != NULL)
		{
			i = atoi(str);
			XMLString::release(&str);
		}
	}

    getNextValue();

    return *this; 
}

CRaptorIO& CXMLRaptorIO::operator<<(float i)  { return *this; }
CRaptorIO& CXMLRaptorIO::operator>>(float& i)  
{ 
	if (currentNode != NULL)
	{
		char *str = XMLString::transcode(currentNode->getNodeValue());

		if (str != NULL)
		{
			i = (float)atof(str);
            XMLString::release(&str);
		}
	}

    getNextValue();

    return *this; 
}

CRaptorIO& CXMLRaptorIO::operator<<(unsigned int i)  { return *this; }
CRaptorIO& CXMLRaptorIO::operator>>(unsigned int& i)  
{ 
	if (currentNode != NULL)
	{
		char *str = XMLString::transcode(currentNode->getNodeValue());

		if (str != NULL)
		{
			i = atoi(str);
			XMLString::release(&str);
		}
	}

    getNextValue();

    return *this; 
}

CRaptorIO& CXMLRaptorIO::operator<<(const std::string& s)  { return *this; }
CRaptorIO& CXMLRaptorIO::operator>>(std::string &s)  
{
	if (currentNode != NULL)
	{
		char *str = XMLString::transcode(currentNode->getNodeValue());
		if (str != NULL)
		{
            s = str;
			XMLString::release(&str);
		}
    }

	// strip separators
	while ((s.length() > 0) && ((s[0] == ' ') || (s[0] == '\t') || (s[0] == '\n')))
		s.erase(0, 1);

	getNextValue();

    return *this;
}

//	Methods
CRaptorIO& CXMLRaptorIO::parse(const char *data, size_t size)
{
    if ((xml_parser == NULL) || (NULL == data))
	    return *this;

	const char *shema = NULL;
	if (0 == strcmp((const char*)data+strlen((const char*)data)-4,".xsd"))
	{
		string msg = "Raptor XMLIO will use noNamespaceShemaLocation file: ";
		msg += data;
		RAPTOR_NO_ERROR(CPersistence::CPersistenceClassID::GetClassId(),msg.c_str());
		xml_parser->setExternalNoNamespaceSchemaLocation(data);
		return *this;
	}

    xml_parser->resetDocumentPool();

    try
    {
        if (getKind() == CRaptorIO::DISK_READ)
        {
            LocalFileInputSource source(XMLString::transcode(data));
            xml_parser->parse(source);
        }
        else if (getKind() == CRaptorIO::MEMORY)
        {
	        MemBufInputSource source((unsigned char*)data,size,"CXMLRaptorIO");
            xml_parser->parse(source);
        }
    }
    catch (const XMLException& e)
    {
        stringstream s;
		s << "An error occurred during parsing\n   Message: ";
        s << XMLString::transcode(e.getMessage());
        RAPTOR_ERROR(CPersistence::CPersistenceClassID::GetClassId(),s.str());
    }
    catch (const DOMException& e)
    {
        stringstream s;
        s << "DOM Error during parsing stream \nDOMException code is:  " << e.code;
        RAPTOR_ERROR(CPersistence::CPersistenceClassID::GetClassId(),s.str());
    }
    catch (...)
    {
        RAPTOR_ERROR(CPersistence::CPersistenceClassID::GetClassId(),"An error occurred during parsing. ");
    }

    DOMNode *document = xml_parser->getDocument();
	currentNode = document->getFirstChild();

    return *this; 
}

CRaptorIO& CXMLRaptorIO::read(void *data, size_t size)
{
	return *this;
}

CRaptorIO& CXMLRaptorIO::write(const void *data, size_t size)
{ 
	return *this; 
}

CRaptorIO& CXMLRaptorIO::seek(size_t size)
{ 
	return *this;
}

string CXMLRaptorIO::getValueName(void) const
{
    std::string str = "";

	if (NULL != currentNode)
	{
        short type = currentNode->getNodeType();
        if ((type == DOMNode::ELEMENT_NODE) || (type == DOMNode::ATTRIBUTE_NODE))
		    str = XMLString::transcode(currentNode->getNodeName());
        else if (type == DOMNode::TEXT_NODE)
            str = XMLString::transcode(currentNode->getNodeValue());
        else
            str = XMLString::transcode(currentNode->getTextContent());
	}

	// strip separators
	while ((str.length() > 0) && ((str[0] == ' ') || (str[0] == '\t') || (str[0] == '\n')))
		str.erase(0, 1);

	return str;
}

//	structures I/O
CRaptorIO& CXMLRaptorIO::operator<<(const GL_COORD_VERTEX_TAG & v)  { return *this; }
CRaptorIO& CXMLRaptorIO::operator>>(GL_COORD_VERTEX_TAG & v)  
{
    std::string name;
	*this >> name;
    
    std::string data = getValueName();
    while (hasMoreValues())
    {
		if (data == "x")
    		*this >> v.x;
		else if (data == "y")
    		*this >> v.y;
		else if (data == "z")
    		*this >> v.z;
		else if (data == "h")
    		*this >> v.h;
		else
			*this >> name;

		data = getValueName();
    }

	// skip to next value
	*this >> name;

	return *this; 
}

CRaptorIO& CXMLRaptorIO::operator<<(const GL_TEX_VERTEX_TAG & v)  { return *this; }
CRaptorIO& CXMLRaptorIO::operator>>(GL_TEX_VERTEX_TAG & v)
{ 
    string name;
	*this >> name;
    
    string data = getValueName();
    while (hasMoreValues())
    {
		if (data == "u")
			*this >> v.u;
		else if (data == "v")
			*this >> v.v;
		else
			*this >> name;

		data = getValueName();
    }

	// skip to next value
	*this >> name;

	return *this; 
}

CRaptorIO& CXMLRaptorIO::operator<<(const GL_MATRIX_TAG & m){ return *this; }
CRaptorIO& CXMLRaptorIO::operator>>(GL_MATRIX_TAG & m)
{ 
    string name;
	*this >> name;
    
    string data = getValueName();
    while (hasMoreValues())
    {
		if (data == "m11")
    		*this >> m.rowx.x;
		else if (data == "m12")
    		*this >> m.rowx.y;
		else if (data == "m13")
    		*this >> m.rowx.z;
		else if (data == "m14")
    		*this >> m.rowx.h;
		else if (data == "m21")
    		*this >> m.rowy.x;
		else if (data == "m22")
    		*this >> m.rowy.y;
		else if (data == "m23")
    		*this >> m.rowy.z;
		else if (data == "m24")
    		*this >> m.rowy.h;
		else if (data == "m31")
    		*this >> m.rowz.x;
		else if (data == "m32")
    		*this >> m.rowz.y;
		else if (data == "m33")
    		*this >> m.rowz.z;
		else if (data == "m34")
    		*this >> m.rowz.h;
		else if (data == "m41")
    		*this >> m.rowh.x;
		else if (data == "m42")
    		*this >> m.rowh.y;
		else if (data == "m43")
    		*this >> m.rowh.z;
		else if (data == "m44")
    		*this >> m.rowh.h;
		else
			*this >> name;

		data = getValueName();
    }

	// skip to next value
	*this >> name;

	return *this; 
}

CRaptorIO& CXMLRaptorIO::operator<<(const CColor::RGBA & c){ return *this; }
CRaptorIO& CXMLRaptorIO::operator>>(CColor::RGBA & c)
{ 
    string name;
	*this >> name;
    
    string data = getValueName();
    while (hasMoreValues())
    {
		if (data == "r")
    		*this >> c.r;
		else if (data == "g")
    		*this >> c.g;
		else if (data == "b")
    		*this >> c.b;
		else if (data == "a")
    		*this >> c.a;
		else
			*this >> name;

		data = getValueName();
    }

	// skip to next value
	*this >> name;

	return *this; 
}

void CXMLRaptorIO::getNextValue(void)
{
    if (nodePos >= 0)
	{
		NODE& node = nodes[nodePos];

		if ((node.attrPos >= 0) && (node.attrPos < node.attrList->getLength()))
		{
			currentNode = node.attrList->item(node.attrPos++);
			return;
		}
		else if ((node.childPos >= 0) && (node.childPos < node.nodeList->getLength()))
		{
			currentNode = node.nodeList->item(node.childPos++);
			// Do not return yet to grab node attributes in first place
			// before descending to childs (see next 'if' below)
		}
		else
		{
			currentNode = NULL;
			nodePos--;
			nodes.pop_back();
			return;
		}
    }
	
	if ((currentNode != NULL) && (currentNode->getNodeType() == DOMNode::ELEMENT_NODE))
		if ((currentNode->hasAttributes()) || (currentNode->hasChildNodes()))
    {
		CXMLRaptorIO::NODE node;
		node.childPos = -1;
		node.attrPos = -1;
		node.attrList = NULL;
		node.nodeList = NULL;

		if (currentNode->hasAttributes())
		{
			node.attrList = currentNode->getAttributes();
			node.attrPos = 0;
		}
    
		if (currentNode->hasChildNodes())
		{	
			node.nodeList = currentNode->getChildNodes();
			node.childPos = 0;
		}
		
		nodes.push_back(node);
		nodePos++;
    }
}

bool CXMLRaptorIO::hasMoreValues(void) const
{
    return (currentNode != NULL);
}



