/***************************************************************************/
/*                                                                         */
/*  Object3D.cpp                                                           */
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

#ifndef __RAPTOR_GLEXT_H__
	#include "System/Glext.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Object3D.h"
#endif
#ifdef RAPTOR_SSE_CODE_GENERATION
	#if !defined(AFX_SSE_BOUNDINGBOX_H__DD1B683A_C345_4399_B1CF_4059B5C3A744__INCLUDED_)
		#include "SSE_Engine/SSE_BoundingBox.h"
	#endif
#endif
#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_OBJECT3DCONTAINERNOTIFIER_H__BF1EABCD_500E_4D7C_8561_2C535DF0640A__INCLUDED_)
    #include "Subsys/Object3DContainerNotifier.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif


//////////////////////////////////////////////////////////////////////
// Static data for bbox vertices
RAPTOR_NAMESPACE_BEGIN


bool CObject3D::earlyClipEnabled = true;

static CObject3D::CObject3DClassID objectID;
const CPersistence::CPersistenceClassID& CObject3D::CObject3DClassID::GetClassId(void)
{
	return objectID;
}


RAPTOR_NAMESPACE_END
//////////////////////////////////////////////////////////////////////

RAPTOR_NAMESPACE

bool CObject3D::isEarlyClipEnabled(void)
{ return earlyClipEnabled; }
 
void CObject3D::enableEarlyClip(bool enable)
{ earlyClipEnabled = enable; };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CObject3D::CObject3D(const CPersistence::CPersistenceClassID & classID,
					const std::string& name)
    :CPersistence(classID,name)
{
#ifdef RAPTOR_SSE_CODE_GENERATION
	if (getCPUINFO().SSE)
		BBox = new CSSE_BoundingBox();
	else
#endif
		BBox = new CBoundingBox();

    earlyClip = C3DEngine::CLIP_UNKNOWN;

	updateBBox = false;
	bbox = UINT64_MAX;
}

CObject3D::~CObject3D()
{
    vector< CContainerNotifier<CObject3D*>* >::iterator it = m_pNotifiers.begin();
    while (it != m_pNotifiers.end())
        (*it++)->notify(CContainerNotifier<CObject3D*>::DESTRUCTION,this);

	//if (NULL != m_pBinder)
	//{
	//	CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder*)m_pBinder;
	//	delete binder;
	//}

	delete BBox;
}

CObject3D& CObject3D::operator=(const CObject3D& rsh)
{
    *BBox = *rsh.BBox;
    properties = rsh.properties;

    notifyBoundingBox();

    return *this;
}

bool CObject3D::addContainerNotifier(CContainerNotifier<CObject3D*> *pNotifier)
{
    if (pNotifier == NULL)
        return false;

    bool res = true;

    vector< CContainerNotifier<CObject3D*>* >::const_iterator it = m_pNotifiers.begin();
    while ((it != m_pNotifiers.end()) && res)
        res = ((*it++) != pNotifier);

    if (res)
        m_pNotifiers.push_back(pNotifier);

    return res;
}

bool CObject3D::removeContainerNotifier(CContainerNotifier<CObject3D*> *pNotifier)
{
    if (pNotifier == NULL)
        return false;

    vector< CContainerNotifier<CObject3D*>* >::iterator it = m_pNotifiers.begin();
    while ((it != m_pNotifiers.end()) && ((*it) != pNotifier))
        it++;

    if (it == m_pNotifiers.end())
        return false;
    else
    {
        m_pNotifiers.erase(it);
        return true;
    }
}

CObject3D::operator RAPTOR_HANDLE() const
{
	RAPTOR_HANDLE handle(getId().ID(),(void*)this);
	return handle;
}

vector<CObject3DContour*> CObject3D::createContours(void)
{
    vector<CObject3DContour*> res;
    return res;
}

void CObject3D::notifyBoundingBox(void)
{
	updateBBox = true;
    vector< CContainerNotifier<CObject3D*>* >::iterator it = m_pNotifiers.begin();
    while (it != m_pNotifiers.end())
        (*it++)->notify(CContainerNotifier<CObject3D*>::UPDATEBBOX,this);
}

void CObject3D::translateCenter(float tx,float ty,float tz)
{
    BBox->translate(tx,ty,tz);
    notifyBoundingBox();
}

void CObject3D::scaleBoundingBox(float sx,float sy,float sz)
{
    BBox->scale(sx,sy,sz);
    notifyBoundingBox();
}

void CObject3D::setBoundingBox(const GL_COORD_VERTEX &Min,const GL_COORD_VERTEX &Max)
{
    BBox->set(Min,Max);
    notifyBoundingBox();
}

void CObject3D::extendBoundingBox(const GL_COORD_VERTEX& min, const GL_COORD_VERTEX& max)
{
	BBox->extendTo(min.x,min.y,min.z);
	BBox->extendTo(max.x,max.y,max.z);
    notifyBoundingBox();
}

void CObject3D::glRenderBBox(RENDER_BOX_MODEL filled)
{
	if (updateBBox)
		glvkUpdateBBox();

	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	
	if (RAW == filled)
	{
		glDrawArrays(GL_LINES, bbox, 2);
	}
	else
	{
		CShader *pShader = instance.m_pWiredBboxShader;
		if (FILLED == filled)
			pShader = instance.m_pFilledBboxShader;

		CResourceAllocator::CResourceBinder *binder = instance.m_pBoxBinder;
		binder->glvkBindArrays();
		pShader->glRender();

		glDrawArrays(GL_LINES, bbox, 2);

		pShader->glStop();
		binder->glvkUnbindArrays();
	}

	instance.iRenderedObjects++;
	instance.iRenderedTriangles += 12;

	CATCH_GL_ERROR
}

void CObject3D::glvkUpdateBBox(void)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
	
	if (instance.numboxes <= bbox)
		bbox = instance.glvkReserveBoxIndex();

	GL_COORD_VERTEX src[2];
	BBox->get(src[0], src[1]);

	float *dst = instance.boxes[bbox];
	// size is 2 coordinates * 4 floats per box, * maxboxes
	size_t sz = 2 * GL_COORD_VERTEX_STRIDE;

	pAllocator->glvkSetPointerData(dst, (float*)src, sz);

	updateBBox = false;
}

void CObject3D::glClipRender(void)
{
    if (earlyClipEnabled && 
       ((earlyClip == C3DEngine::CLIP_NONE) || (earlyClip == C3DEngine::CLIP_PARTS)))
    {
        glRender();
    }
    else
    {
		if (C3DEngine::CLIP_FULL != glClip())
			glRender(); 
    }
}

C3DEngine::CLIP_RESULT RAPTOR_FASTCALL CObject3D::glClip() const
{
	if (this->properties.getClippingMethod() == CObjectProperties::CLIP_AA_BBOX)
		return C3DEngine::Get3DEngine()->glClip(BBox,true,NULL);
	else if (this->properties.getClippingMethod() == CObjectProperties::CLIP_BBOX_OCCLUSION)
		return C3DEngine::Get3DEngine()->glClip(BBox,true,NULL);
	else if (this->properties.getClippingMethod() == CObjectProperties::CLIP_BSPHERE)
		return C3DEngine::Get3DEngine()->glClip(BBox,true);
	else
		return C3DEngine::Get3DEngine()->glClip(BBox,true,NULL);
}

bool RAPTOR_FASTCALL CObject3D::fullClip(const CGenericMatrix<float> &transform)
{
    /*
	earlyClip = Raptor::Get3DEngine()->glClip(BBox,false,&transform);
    return (earlyClip != C3DEngine::CLIP_UNKNOWN);
    */
    earlyClip = C3DEngine::CLIP_UNKNOWN;
    return false;
}


bool CObject3D::exportObject(CRaptorIO& o)
{
		return CPersistence::exportObject(o);
}

bool CObject3D::importObject(CRaptorIO& io)
{
    string name;
    io >> name;

	string data = io.getValueName();
    while (io.hasMoreValues())
    {
		if (data == "name")
			CPersistence::importObject(io);
        else if (data == "Properties")
            properties.importObject(io);
        else
			io >> name;

		data = io.getValueName();
    }
	io >> name;

	return true;
}

