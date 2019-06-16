#include "Subsys/CodeGeneration.h"

#ifndef __GLOBAL_H__
    #include "Global.h"
#endif
#if !defined(AFX_CONTEXTMANAGER_H__F992F5F0_D8A5_475F_9777_B0EB30E7648E__INCLUDED_)
	#include "Subsys/ContextManager.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif




RAPTOR_NAMESPACE

Global	*Global::pInstance = NULL;

Global::Global()
{
}

Global::~Global()
{
    Raptor::glPurgeRaptor(false);
}

Global&	Global::GetInstance()
{
	if (pInstance == NULL)
		pInstance = new Global();

	return *pInstance;
}

bool Global::destroy(void)
{
    if (pInstance == NULL)
        return false;

    delete pInstance;
    pInstance = NULL;

    return true;
}

bool Global::init(const CRaptorConfig& config)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();

	if (!instance.initialised)
	{
		instance.config = config;
		instance.initInstance();
	}

	return instance.initialised;
}

