// Win32RaptorIO.cpp: implementation of the CWin32RaptorIO class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif
#if !defined(AFX_WIN32RAPTORIO_H__9E7680D9_81FA_411E_8570_E2CFE06B8F20__INCLUDED_)
    #include "Win32RaptorIO.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif

#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWin32RaptorIO::CWin32RaptorIO(const std::string& streamName, CRaptorIO::IO_KIND kind)
	:CRaptorIO(streamName,kind)
{
	if (CRaptorIO::IO_FAILED == getStatus())
	{
		DWORD err = ::GetLastError();
		if (err != ERROR_SUCCESS)
		{
			LPVOID lpMsgBuf = NULL;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
						  FORMAT_MESSAGE_FROM_SYSTEM |
						  FORMAT_MESSAGE_IGNORE_INSERTS,
						  NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
						  (LPTSTR)&lpMsgBuf, 0, NULL);

			const char *msg = (NULL != lpMsgBuf) ? (const char*)lpMsgBuf : "Unknown Win32 error.";
			Raptor::GetErrorManager()->generateRaptorError(CPersistence::CPersistenceClassID::GetClassId(),
														   CRaptorErrorManager::RAPTOR_WARNING, msg);

			if (NULL != lpMsgBuf)
				LocalFree(lpMsgBuf);
			SetLastError(0);
		}
	}
}



CRaptorIO& CWin32RaptorIO::operator>>(std::string & s)
{
    m_status = IO_FAILED;

    if (getKind() == DISK_READ)
    {
        char buffer[MAX_PATH];
	    m_inFile.getline(buffer,MAX_PATH,'\n');
        if (m_inFile.good())
            m_status = IO_OK;
        size_t len = strlen(buffer);
        if (len > 0)
            if (buffer[len-1] == 0x0d)
                buffer[len-1] = 0;
        s = buffer;
    }
    
	return *this;
}

