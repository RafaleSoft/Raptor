// RaptorIO.cpp: implementation of the CRaptorIO class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "RaptorIO.h"
#endif

#if defined(_WIN32)
    #include "Win32Specific/Win32RaptorIO.h"
#endif
#if !defined(AFX_XMLRAPTORIO_H__DD049FDC_A319_4028_9759_E858401EB55E__INCLUDED_)
    #include "Subsys/XMLRaptorIO.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif

#include <direct.h>

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaptorIO::CRaptorIO(const std::string& streamName, CRaptorIO::IO_KIND kind)
{
	m_status = IO_FAILED;
	m_kind = kind;
	m_size = 0;

	if (!streamName.empty())
	{
		switch(kind)
		{
			case DISK_READ:
			{
				m_inFile.open(streamName.data(),ios_base::in|ios::binary);
				m_inFile.seekg (0, ios_base::end);
				m_size = m_inFile.tellg();
				m_inFile.seekg (0, ios_base::beg);
				if (m_inFile.good())
					m_status = IO_OK;
				break;
			}
			case DISK_WRITE:
			{
				m_outFile.open(streamName.data(),ios::out|ios::binary);
				if (m_outFile.good())
					m_status = IO_OK;
				break;
			}
			default:
			{
	#ifdef RAPTOR_DEBUG_MODE_GENERATION
				Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
																CRaptorErrorManager::RAPTOR_WARNING,
																"RaptorIO do not support io kind!");
	#endif
				break;
			}
		}
	}
}

CRaptorIO::~CRaptorIO()
{
	switch(getKind())
	{
		case DISK_READ:
			m_inFile.close();
			break;
		case DISK_WRITE:
			m_outFile.close();
			break;
		default:
			break;
	}
}


CRaptorIO* CRaptorIO::Create(const std::string& streamName, IO_KIND kind, CRaptorIO::IO_FORMAT format)
{
	if (streamName.empty())
		return NULL;

    CRaptorIO *res = NULL;

#if defined(_WIN32)
    if (format == ASCII_XML)
        res = new CXMLRaptorIO(streamName,kind);
    else
	    res = new CWin32RaptorIO(streamName,kind);
#else
    if (format == ASCII_XML)
        res = new CXMLRaptorIO(streamName,kind);
    else
	    res = new CRaptorIO(streamName,kind);
#endif

    return res;
}

unsigned int CRaptorIO::getSize(void) const
{
	if (getKind() == DISK_READ)
		return m_size; 
	else if (getKind() == DISK_WRITE)
    {
		if (m_outFile.good())
			return 0;
			//! Implementation use a non const method to return offset !!!
			//return m_outFile.tellp();
		else
			return 0;
	}
	else
		return 0;
};

//  Extensible io management
std::string CRaptorIO::getValueName(void) const 
{ 
	return NULL; 
};


bool CRaptorIO::hasMoreValues(void) const
{
    //! Status should be OK until we reach eof, thus no more data.
    return (m_status == IO_OK);
}

CRaptorIO& CRaptorIO::operator<<(const CColor::RGBA & c)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_WRITE)
    {
	    m_outFile << c.r << " " << c.g << " " << c.b << " " << c.a << endl; 
        if (m_outFile.good())
            m_status = IO_OK;
    }
    
	return *this;
}
	
CRaptorIO& CRaptorIO::operator>>(CColor::RGBA & c)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_READ)
    {
	    m_inFile >> c.r >> c.g >> c.b >> c.a;
        if (m_inFile.good())
            m_status = IO_OK;
    }
    
	return *this;
}

	
CRaptorIO& CRaptorIO::operator<<(const GL_MATRIX_TAG & m)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_WRITE)
    {
	    m_outFile << m.rowx << m.rowy << m.rowz << m.rowh;
        if (m_outFile.good())
            m_status = IO_OK;
    }

	return *this;
}

CRaptorIO& CRaptorIO::operator>>(GL_MATRIX_TAG& m)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_READ)
    {
	    *this >> m.rowx >> m.rowy >> m.rowz >> m.rowh;
        if (m_inFile.good())
            m_status = IO_OK;
    }
    	
	return *this;
}


CRaptorIO& CRaptorIO::operator<<(const GL_TEX_VERTEX_TAG & v)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_WRITE)
    {
	    m_outFile << v.u << " " << v.v << endl;
        if (m_outFile.good())
            m_status = IO_OK;
    }

	return *this;
}

CRaptorIO& CRaptorIO::operator>>(GL_TEX_VERTEX_TAG & v)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_READ)
    {
	    m_inFile >> v.u >> v.v;
        if (m_inFile.good())
            m_status = IO_OK;
    }

	return *this;
}

CRaptorIO& CRaptorIO::operator<<(const GL_COORD_VERTEX_TAG & v)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_WRITE)
    {
	    m_outFile << v.x << " " << v.y << " " << v.z << " " << v.h << endl;
        if (m_outFile.good())
			m_status = IO_OK;
    }

	return *this;
}

CRaptorIO& CRaptorIO::operator>>(GL_COORD_VERTEX_TAG & v)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_READ)
    {
	    m_inFile >> v.x >> v.y >> v.z >> v.h;
        if (m_inFile.good())
            m_status = IO_OK;
    }

	return *this;
}



CRaptorIO& CRaptorIO::write(const void *data,size_t size)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_WRITE)
    {
	    m_outFile.write((const char*)data,size);
        if (m_outFile.good())
            m_status = IO_OK;
    }

	return *this;
}

CRaptorIO& CRaptorIO::read(void *data, size_t size)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_READ)
    {
	    m_inFile.read((char*)data,size);
        if (m_inFile.good())
            m_status = IO_OK;
    }   

	return *this;
}

CRaptorIO& CRaptorIO::seek(size_t size)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_READ)
    {
		m_inFile.seekg(size,ios_base::cur);
        if (m_inFile.good())
            m_status = IO_OK;
    }   

	return *this;
}


CRaptorIO& CRaptorIO::operator<<(bool i)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_WRITE)
    {
        m_outFile << i;
        if (m_outFile.good())
            m_status = IO_OK;
    }
    
	return *this;
}

CRaptorIO& CRaptorIO::operator>>(bool& i)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_READ)
    {
	    m_inFile >> i;
        if (m_inFile.good())
            m_status = IO_OK;
    }
    
	return *this;
}

CRaptorIO& CRaptorIO::operator<<(char i)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_WRITE)
    {
        m_outFile << i;
        if (m_outFile.good())
            m_status = IO_OK;
    }
    
	return *this;
}

CRaptorIO& CRaptorIO::operator>>(char& i)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_READ)
    {
	    m_inFile >> i;
        if (m_inFile.good())
            m_status = IO_OK;
    }
    
	return *this;
}

CRaptorIO& CRaptorIO::operator<<(int i)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_WRITE)
    {
	    m_outFile << i ;
        if (m_outFile.good())
            m_status = IO_OK;
    }
    
	return *this;
}

CRaptorIO& CRaptorIO::operator>>(int& i)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_READ)
    {
	    m_inFile >> i;
        if (m_inFile.good())
            m_status = IO_OK;
    }
    
	return *this;
}

CRaptorIO& CRaptorIO::operator<<(unsigned short s)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_WRITE)
    {
	    m_outFile << s;
        if (m_outFile.good())
            m_status = IO_OK;
    }
    
	return *this;
}

CRaptorIO& CRaptorIO::operator>>(unsigned short& s)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_READ)
    {
	    m_inFile >> s;
        if (m_inFile.good())
            m_status = IO_OK;
    }
    
	return *this;
}

CRaptorIO& CRaptorIO::operator<<(unsigned int i)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_WRITE)
    {
	    m_outFile << i ;
        if (m_outFile.good())
            m_status = IO_OK;
    }
    
	return *this;
}

CRaptorIO& CRaptorIO::operator>>(unsigned int& i)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_READ)
    {
	    m_inFile >> i;
        if (m_inFile.good())
            m_status = IO_OK;
    }
    
	return *this;
}

CRaptorIO& CRaptorIO::operator<<(float f)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_WRITE)
    {
	    m_outFile << f;
        if (m_outFile.good())
            m_status = IO_OK;
    }
    
	return *this;
}

CRaptorIO& CRaptorIO::operator>>(float& f)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_READ)
    {
	    m_inFile >> f;
        if (m_inFile.good())
            m_status = IO_OK;
    }
    
	return *this;
}

CRaptorIO& CRaptorIO::operator<<(const std::string& s)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_WRITE)
    {
	    m_outFile << s;
        if (m_outFile.good())
            m_status = IO_OK;
    }
    
	return *this;
}


CRaptorIO& CRaptorIO::operator>>(std::string & s)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_READ)
    {
        char buffer[MAX_PATH];
	    m_inFile.getline(buffer,MAX_PATH,'\n');
        if (m_inFile.good())
            m_status = IO_OK;

        s = buffer;
    }
    
	return *this;
}

