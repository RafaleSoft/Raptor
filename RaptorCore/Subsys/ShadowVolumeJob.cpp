// ShadowVolumeJob.cpp: implementation of the CShadowVolumeJob class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_SHADOWVOLUMEJOB_H__3C3EB9C3_88DF_4C32_B66E_5265B65D8648__INCLUDED_)
    #include "ShadowVolumeJob.h"
#endif
#if !defined(AFX_OBJECT3DSHADOW_H__A502A78F_0575_4F30_BCE2_0658B60EB490__INCLUDED_)
	#include "GLHierarchy/Object3DShadow.h"
#endif
#if !defined(AFX_OBJECT3DSHADOWATTRIBUTES_H__4FAFE331_A695_471C_AD48_0F541948AAAD__INCLUDED_)
    #include "Subsys/Object3DShadowAttributes.h"
#endif
#if !defined(AFX_OBJECT3DCONTOUR_H__C0C2B562_ABBC_4B04_A1E7_E0727FAC66AB__INCLUDED_)
	#include "GLHierarchy/Object3DContour.h"
#endif

RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShadowVolumeJob::CShadowVolumeJob(CObject3DShadow* owner,unsigned int batchId)
    :CEngineJob(batchId),shadow(owner)
{

}

CShadowVolumeJob::~CShadowVolumeJob()
{

}

bool CShadowVolumeJob::computeJob(CGenericMatrix<float> &transform)
{
    CObject3DContour *pContour = shadow->m_pVisibleContours[0];
    if ((!pContour->getContourVolume().darkCapClipped) ||
        (!pContour->getContourVolume().lightCapClipped))
        shadow->buildShadow(transform);

	jobDone();

    return true;
}



