// XMLRaptorIO.h: interface for the CXMLRaptorIO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLRAPTORIO_H__DD049FDC_A319_4028_9759_E858401EB55E__INCLUDED_)
#define AFX_XMLRAPTORIO_H__DD049FDC_A319_4028_9759_E858401EB55E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
    #include "System/RaptorIO.h"
#endif


#include <xercesc/parsers/XercesDOMParser.hpp>


RAPTOR_NAMESPACE_BEGIN


class CXMLRaptorIO : public CRaptorIO  
{
public:
    CXMLRaptorIO(const char *streamName,CRaptorIO::IO_KIND kind);
	virtual ~CXMLRaptorIO();

	//	Specific XML operations
	virtual std::string getValueName(void) const;
    virtual bool hasMoreValues(void) const;

    //	Basic types operators
	virtual CRaptorIO& operator<<(bool i) ;
	virtual CRaptorIO& operator>>(bool& i) ;

	virtual CRaptorIO& operator<<(char i)  ;
	virtual CRaptorIO& operator>>(char& i)  ;

	virtual CRaptorIO& operator<<(unsigned short i)  ;
	virtual CRaptorIO& operator>>(unsigned short& i)  ;

	virtual CRaptorIO& operator<<(int i)  ;
	virtual CRaptorIO& operator>>(int& i)  ;

	virtual CRaptorIO& operator<<(float i)  ;
	virtual CRaptorIO& operator>>(float& i)  ;

	virtual CRaptorIO& operator<<(unsigned int i)  ;
	virtual CRaptorIO& operator>>(unsigned int& i)  ;

	virtual CRaptorIO& operator<<(const std::string& s)  ;
	virtual CRaptorIO& operator>>(std::string& s)  ;

	//	Methods
	virtual CRaptorIO& write(const void *data,unsigned int size)  ;
	virtual CRaptorIO& read(void *data,unsigned int size)  ;
	virtual CRaptorIO& seek(unsigned int size) ;


	//	structures I/O
	virtual CRaptorIO& operator<<(const GL_COORD_VERTEX_TAG & v)  ;
	virtual CRaptorIO& operator>>(GL_COORD_VERTEX_TAG & v)  ;
	virtual CRaptorIO& operator<<(const GL_TEX_VERTEX_TAG & v)  ;
	virtual CRaptorIO& operator>>(GL_TEX_VERTEX_TAG & v)  ;
	virtual CRaptorIO& operator<<(const GL_MATRIX_TAG & m);
	virtual CRaptorIO& operator>>(GL_MATRIX_TAG & m);
	virtual CRaptorIO& operator<<(const CColor::RGBA & c);
	virtual CRaptorIO& operator>>(CColor::RGBA & c);

private:
    void        getNextValue(void);

    //!    The parser for this IO.
    xercesc::XercesDOMParser *xml_parser;

    //!    Node position for current parsed value.
    int         nodePos;

    //!    Parsing pointers
    xercesc::DOMNode       *currentNode;
    xercesc::DOMNode       *parentNode;

    typedef struct NODE_t
    {
        xercesc::DOMNodeList		*nodeList;
        xercesc::DOMNamedNodeMap	*attrList;
	    int							attrPos;
        int							childPos;
    } NODE;
    vector<NODE> nodes;


    //!    A static counter to terminate XML platform.
    static    unsigned int terminateCount;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_XMLRAPTORIO_H__DD049FDC_A319_4028_9759_E858401EB55E__INCLUDED_)

