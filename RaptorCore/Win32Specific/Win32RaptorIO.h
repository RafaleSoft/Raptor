// Win32RaptorIO.h: interface for the CWin32RaptorIO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WIN32RAPTORIO_H__9E7680D9_81FA_411E_8570_E2CFE06B8F20__INCLUDED_)
#define AFX_WIN32RAPTORIO_H__9E7680D9_81FA_411E_8570_E2CFE06B8F20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
    #include "System/RaptorIO.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class CWin32RaptorIO : public CRaptorIO  
{
public:
	CWin32RaptorIO(const char *streamName,CRaptorIO::IO_KIND kind);
	virtual ~CWin32RaptorIO() {};

	//	Win32 EOL has 0xa followed by 0xd
	virtual CRaptorIO& operator>>(std::string & s);

};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_WIN32RAPTORIO_H__9E7680D9_81FA_411E_8570_E2CFE06B8F20__INCLUDED_)

