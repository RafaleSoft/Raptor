/***************************************************************************/
/*                                                                         */
/*  RaptorInstance.cpp                                                     */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#if !defined(AFX_RAPTORDATAMANAGER_H__114BFB19_FA00_4E3E_879E_C9130043668E__INCLUDED_)
	#include "DataManager/RaptorDataManager.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORCONFIG_H__29B753B8_17DE_44DF_A4D2_9D19C5AC53D5__INCLUDED_)
	#include "System/RaptorConfig.h"
#endif


RAPTOR_NAMESPACE


CRaptorInstance *CRaptorInstance::m_pInstance = NULL;

CRaptorInstance::CRaptorInstance()
{
	p3DEngine = NULL;
	pMessages = NULL;
	pErrorMgr = NULL;
}

CRaptorInstance::~CRaptorInstance()
{
	if (pMessages != NULL)
		delete pMessages;
	if (pErrorMgr != NULL)
		delete pErrorMgr;

	delete CRaptorDataManager::GetInstance();
}

CRaptorInstance &CRaptorInstance::GetInstance(void)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CRaptorInstance();
	}

	return *m_pInstance;
}

void CRaptorInstance::initInstance()
{
	//!	Initialize error manager.
	pErrorMgr = new CRaptorErrorManager();
	pErrorMgr->logToFile(config.m_logFile);

	//! Initialise error mesasges.
	pMessages = new CRaptorMessages();
	CRaptorDataManager  *dataManager = CRaptorDataManager::GetInstance();
	if (dataManager != NULL)
	{
		string filepath = dataManager->ExportFile("RaptorMessages.xml");
		if (!filepath.empty())
			pMessages->LoadMessages(filepath);
	}
}