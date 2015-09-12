// LightGlow.cpp: implementation of the CLightGlow class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_LIGHTGLOW_H__577C39B3_EE0B_4A07_8974_BC250BA2960A__INCLUDED_)
    #include "LightGlow.h"
#endif


RAPTOR_NAMESPACE_BEGIN

static CLightGlow::CPersistenceClassID lightglowId;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightGlow::CLightGlow(const std::string& name):
    CPersistence(lightglowId,name)
{
}

CLightGlow::~CLightGlow(void)
{
}
