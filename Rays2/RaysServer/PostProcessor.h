// PostProcessor.h: interface for the CPostProcessor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POSTPROCESSOR_H__8D096A50_00CA_4107_9B78_F60CA870AB8B__INCLUDED_)
#define AFX_POSTPROCESSOR_H__8D096A50_00CA_4107_9B78_F60CA870AB8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>	// CArray
#include "..\Plugin.h"
#include "RaysServer.h"

class CPostProcessor  
{
public:
	CPostProcessor(LPJOBSTRUCT pOwnerJob):
	  m_pOwnerJob(pOwnerJob),mpThread(NULL) {};

	virtual ~CPostProcessor();

	void AddData(MSGSTRUCT& msg,unsigned char raw_data[],CFileManager* pFileManager);

	void Run(void);

private:
	typedef struct after_effect
	{
		PLUGIN_t		module;
		rays_plugin_t	call;
	} after_effect_t;

	CWinThread*		mpThread;
	rays_config_t	mConfig;
	LPJOBSTRUCT		m_pOwnerJob;

	CArray<after_effect_t,after_effect_t&>	effects;

	void ApplyAfterEffects(void);
	friend UINT runAfterEffects(LPVOID pParam);
};

#endif // !defined(AFX_POSTPROCESSOR_H__8D096A50_00CA_4107_9B78_F60CA870AB8B__INCLUDED_)
