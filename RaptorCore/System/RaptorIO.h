// RaptorIO.h: interface for the CRaptorIO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
#define AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#ifndef __CGLTYPES_HPP__
    #include "System/CGLTypes.h"
#endif

#include <fstream>


RAPTOR_NAMESPACE_BEGIN


//! This class defines IO operations necessary for Raptor. These operations
//! can be specialized using kind and format.
class RAPTOR_API CRaptorIO  
{
public:
	typedef enum
	{
		DISK_READ,
		DISK_WRITE,
		MEMORY,
		NETWORK
	} IO_KIND;

    typedef enum
    {
        ASCII_TEXT,
        ASCII_XML,
		BINARY
    } IO_FORMAT;

    typedef enum
    {
        IO_FAILED,
        IO_OK
    } IO_STATUS;

public:
    //! The only way to create the appropriate IO handler.
	static CRaptorIO* Create(const std::string& streamName, IO_KIND kind, IO_FORMAT format = ASCII_TEXT);
	virtual ~CRaptorIO();

    //! Returns the kind of this IO
	IO_KIND	getKind(void) const { return m_kind; };

    //! Returns the status of this IO
    IO_STATUS	getStatus(void) const { return m_status; };

	//! Returns the size of this IO if available (not a stream)
	size_t	getSize(void) const;

	//!	Defines the flush logic: autoflushed when internal buffers are
	//! full, or systematic flush at each file write.
	//! @param flush: if true, flush is automatic, else it is at each write.
	void setAutoFlush(bool flush) { m_bAutoflush = flush; }

    //!  Extensible io management
	virtual std::string getValueName(void) const;

    //! This method can be used to traverse easily a tree
    virtual bool hasMoreValues(void) const;
    
	//!	Basic types operators
	virtual CRaptorIO& operator<<(bool i);
	virtual CRaptorIO& operator>>(bool& i);

	virtual CRaptorIO& operator<<(char i);
	virtual CRaptorIO& operator>>(char& i);

	virtual CRaptorIO& operator<<(unsigned short i);
	virtual CRaptorIO& operator>>(unsigned short& i);

	virtual CRaptorIO& operator<<(int i);
	virtual CRaptorIO& operator>>(int& i);

	virtual CRaptorIO& operator<<(float i);
	virtual CRaptorIO& operator>>(float& i);

	virtual CRaptorIO& operator<<(unsigned int i);
	virtual CRaptorIO& operator>>(unsigned int& i);

	virtual CRaptorIO& operator<<(const std::string& s);
	virtual CRaptorIO& operator>>(string &s);

	//! To avoid operator<<(bool) beeing invokd !!!
	CRaptorIO& operator<<(const char *c_str)
	{ if (c_str != NULL) return this->operator<<(string(c_str)); else return *this; };

	//!	Methods
	virtual CRaptorIO& write(const void *data,size_t size);
	virtual CRaptorIO& read(void *data,size_t size);
	virtual CRaptorIO& seek(size_t size);
	virtual CRaptorIO& parse(const char *data,size_t size) { return *this; };


	//!	structures I/O
	virtual CRaptorIO& operator<<(const GL_COORD_VERTEX_TAG & v);
	virtual CRaptorIO& operator>>(GL_COORD_VERTEX_TAG & v);
	virtual CRaptorIO& operator<<(const GL_TEX_VERTEX_TAG & v);
	virtual CRaptorIO& operator>>(GL_TEX_VERTEX_TAG & v);
	virtual CRaptorIO& operator<<(const GL_MATRIX_TAG & m);
	virtual CRaptorIO& operator>>(GL_MATRIX_TAG & m);
	virtual CRaptorIO& operator<<(const CColor::RGBA & c);
	virtual CRaptorIO& operator>>(CColor::RGBA & c);

protected:
	CRaptorIO(const std::string& streamName, CRaptorIO::IO_KIND kind);

    IO_STATUS		m_status;
	std::streampos	m_size;
	std::ofstream	m_outFile;
	std::ifstream	m_inFile;

private:
	IO_KIND		m_kind;
	bool		m_bAutoflush;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)

