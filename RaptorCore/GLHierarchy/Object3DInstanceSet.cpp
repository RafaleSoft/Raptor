// Object3DInstanceSet.cpp: implementation of the CObject3DInstanceSet class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_OBJECT3DINSTANCESET_H__4552BADA_3CE7_40A5_9FE3_C10D58360193__INCLUDED_)
	#include "Object3DInstanceSet.h"
#endif
#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/3DEngine.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "ObjectFactory.h"
#endif
#if !defined(AFX_3DENGINEMATRIX_H__6CD1110E_1174_4f38_A452_30FB312022D0__INCLUDED_)
	#include "Engine/3DEngineMatrix.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static CObject3DInstanceSet::CObject3DInstanceSetClassID instanceId;
static CPersistentObjectType<CObject3DInstanceSet> instanceSetFactory(instanceId);

CObject3DInstanceSet::CObject3DInstanceSet(const std::string& name)
:CObject3DInstance(instanceId,name)
{
    m_finalTransform.Ident();
}

CObject3DInstanceSet::~CObject3DInstanceSet()
{

}

void CObject3DInstanceSet::Instantiate(CObject3D * const instance,const GL_MATRIX &transform)
{
	if (instance != NULL)
	{
		ObjectInstance objectInstance;

		GL_COORD_VERTEX bMin;
		GL_COORD_VERTEX bMax;
        CBoundingBox tmpBox = *boundingBox();


        
        CGenericMatrix<float> t;
        t = transform;
        m_finalTransform = t * m_finalTransform;
		C3DEngineMatrix it(m_transform);
		it.Transpose() *= m_finalTransform;
        GL_MATRIX _t;
        C3DEngine::Generic_to_MATRIX(_t,it);
		instance->boundingBox()->get(bMin,bMax,_t);

        tmpBox.extendTo(bMin.x,bMin.y,bMin.z);
        tmpBox.extendTo(bMax.x,bMax.y,bMax.z);
        tmpBox.get(bMin,bMax);
        setBoundingBox(bMin,bMax);

		C3DEngineMatrix transformTranspose(transform);
		C3DEngine::Generic_to_MATRIX(objectInstance.transform,transformTranspose.Transpose());
		objectInstance.pObject = instance;

		m_pObjects.push_back(objectInstance);
	}
}

void CObject3DInstanceSet::glRender(void)
{
	if (!properties.isVisible())
		return;

	if (m_pObjects.size() > 0)
	{
		glPushMatrix();
		glMultMatrixf(m_transform);

		for (unsigned int i=0;i<m_pObjects.size();i++)
		{
			const ObjectInstance &objectInstance = m_pObjects[i];

			glMultMatrixf(objectInstance.transform);
			objectInstance.pObject->glRender();
		}

		glPopMatrix();
	}

	CATCH_GL_ERROR
}

bool RAPTOR_FASTCALL CObject3DInstanceSet::fullClip(const CGenericMatrix<float> &transform)
{
	if (m_pObjects.size() > 0)
	{
        CGenericMatrix<float> boxTransform = transform;
		C3DEngine *pEngine = C3DEngine::Get3DEngine();

        earlyClip = pEngine->glClip(boundingBox(),false,&boxTransform);

		C3DEngineMatrix it(m_transform);
		boxTransform *= it.Transpose();

		for (unsigned int i=0;i<m_pObjects.size();i++)
		{
			const ObjectInstance &objectInstance = m_pObjects[i];

			it = objectInstance.transform;
			boxTransform *= it.Transpose();
            objectInstance.pObject->fullClip(boxTransform);
		}

		return (earlyClip != C3DEngine::CLIP_UNKNOWN);
	}
	else
		return CObject3D::fullClip(transform);
}

void CObject3DInstanceSet::glClipRender(void)
{
	if (!properties.isVisible())
		return;

   	C3DEngine::CLIP_RESULT clip = this->glClip();
    if (clip == C3DEngine::CLIP_FULL)
		return;

	if (m_pObjects.size() > 0)
	{
		glPushMatrix();
		glMultMatrixf(m_transform);

		for (unsigned int i=0;i<m_pObjects.size();i++)
		{
			const ObjectInstance &objectInstance = m_pObjects[i];

			glMultMatrixf(objectInstance.transform);
            if (clip == C3DEngine::CLIP_NONE)
                objectInstance.pObject->glRender();
            else
			    objectInstance.pObject->glClipRender();
		}

		glPopMatrix();
	}

	CATCH_GL_ERROR
}

void CObject3DInstanceSet::glRenderBBox(bool filled)
{
	if (!properties.isVisible())
		return;

	if (m_pObjects.size() > 0)
	{
		glPushMatrix();
		glMultMatrixf(m_transform);

		for (unsigned int i=0;i<m_pObjects.size();i++)
		{
			const ObjectInstance &objectInstance = m_pObjects[i];

			glMultMatrixf(objectInstance.transform);
			objectInstance.pObject->glRenderBBox(filled);
		}

		glPopMatrix();
	}

    CATCH_GL_ERROR
}

