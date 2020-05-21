// OmniShadowMap.cpp: implementation of the COmniShadowMap class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_AMBIENTOCCLUSION_H__6BE8DC67_E93D_4BBB_B72E_A1A5F0263E5E__INCLUDED_)
    #include "AmbientOcclusion.h"
#endif
#if !defined(AFX_AMBIENTOCCLUSIONSHADER_H__FA8234C4_82B1_49D3_ABAA_7FCE45EDDCAD__INCLUDED_)
	#include "Subsys/AmbientOcclusionShader.h"
#endif
#if !defined(AFX_FRAGMENTSHADER_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
    #include "GLHierarchy/FragmentShader.h"
#endif
#if !defined(AFX_VERTEXSHADER_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
	#include "GLHierarchy/VertexShader.h"
#endif
#if !defined(AFX_3DSCENEOBJECT_H__96A34268_AD58_4F73_B633_F6C3E92FE0A9__INCLUDED_)
	#include "Subsys/3DSceneObject.h"
#endif
#if !defined(AFX_SHADEDGEOMETRY_H__E56C66F7_2DF6_497B_AA0F_19DDC11390F9__INCLUDED_)
    #include "GLHierarchy/ShadedGeometry.h"
#endif
#if !defined(AFX_OBJECT3DINSTANCE_H__A2627662_F5F9_11D3_9142_CFEB8E9F2745__INCLUDED_)
	#include "GLHierarchy/Object3DInstance.h"
#endif
#if !defined(AFX_3DSET_H__DB24F01B_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/3DSet.h"
#endif
#if !defined(AFX_GEOMETRYEDITOR_H__2D77E428_ED3D_416B_8DE9_DABFD45A38A7__INCLUDED_)
    #include "GLHierarchy/GeometryEditor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAmbientOcclusion::CAmbientOcclusion(C3DScene& rScene)
	:CEnvironment(rScene)
{
}

CAmbientOcclusion::~CAmbientOcclusion()
{
	unLinkEnvironment();
}

void CAmbientOcclusion::unLinkEnvironment(void)
{
	vector<CAmbientOcclusionShader*>::iterator itr = m_AOShaders.begin();
	while (itr != m_AOShaders.end())
	{
		CAmbientOcclusionShader *pShader = *itr++;
		pShader->unregisterDestruction(m_pObserver);
	}

	CATCH_GL_ERROR
}

void CAmbientOcclusion::unLink(const CPersistence *object)
{
	if ((object != NULL) && (object->getId().isSubClassOf(CShader::CShaderClassID::GetClassId())))
	{
		const CShader *child = static_cast<const CShader*>(object);
		vector<CAmbientOcclusionShader*>::iterator itr = m_AOShaders.begin();
		while (itr != m_AOShaders.end())
		{
			CAmbientOcclusionShader *pShader = *itr;
			if (child == static_cast<const CShader*>(pShader))
			{
				m_AOShaders.erase(itr);
				break;
			}
			itr++;
		}
	}
	else
		CEnvironment::unLink(object);
}


bool CAmbientOcclusion::glInitialize(uint32_t width, uint32_t height)
{
	return true;
}

void CAmbientOcclusion::addObject(C3DSceneObject* object)
{
    CObject3D *obj = object->getObject();

	//	Manage only shadow receivers
	if (!obj->getProperties().isReceiveShadow())
		return;

	if (obj->getId().isSubClassOf(CShadedGeometry::CShadedGeometryClassID::GetClassId()))
	{
		CShadedGeometry* shaded = (CShadedGeometry*)obj;
		registerForAmbientOcclusion(shaded,NULL);
    }
	else if (obj->getId().isSubClassOf(C3DSet::C3DSetClassID::GetClassId()))
	{
		// TODO
	}
	else if (obj->getId().isSubClassOf(CObject3DInstance::CObject3DInstanceClassID::GetClassId()))
	{
		CObject3DInstance* inst = (CObject3DInstance*)obj;
		CObject3D *o = inst->getObject();
		if (o->getId().isSubClassOf(CShadedGeometry::CShadedGeometryClassID::GetClassId()))
		{
			CShadedGeometry* shaded = (CShadedGeometry*)o;
			registerForAmbientOcclusion(shaded,inst);
		}
	}
}

bool CAmbientOcclusion::glInitEnvironment(const vector<C3DSceneObject*>& objects)
{
	vector<C3DSceneObject*>::const_iterator itr = objects.begin();
	while (itr != objects.end())
		addObject(*itr++);

	return true;
}

bool CAmbientOcclusion::registerForAmbientOcclusion(CShadedGeometry* shaded,
													CObject3DInstance* tSource)
{
	//	Prepare geometry for ambient occlusion
	const CGeometryEditor& editor = shaded->getEditor();
	if (!editor.isMinimized())
		editor.minimize();
	if (!editor.hasSurfaceElements())
		editor.genSurfaceElements();
	shaded->addModel(CGeometry::CGL_COLORS);

	//	Now add the ambient occlusion shader
	CShader* AO = shaded->getAmbientOcclusionShader();
	if (AO != NULL)
	{
		CAmbientOcclusionShader* ambientOcclusion = (CAmbientOcclusionShader*)AO;
		ambientOcclusion->glInitAOCompute();
		ambientOcclusion->registerDestruction(m_pObserver);
		ambientOcclusion->setTransformSource(tSource);
		m_AOShaders.push_back(ambientOcclusion);
		return true;
	}
	else 
		return false;
}

void CAmbientOcclusion::glRender(const CLight* currentLight,const vector<C3DSceneObject*>& objects)
{
	if (!isEnabled())
    {
        glRenderObjects(objects, C3DScene::FULL_PASS);
        return;
    }

	// First step: compute relative transforms and occluders
	vector<CAmbientOcclusionShader*> occluders;
	vector<CGenericMatrix<float>> transforms;
	
	// Second step: render AO shaders to compute real time AO
//
// TODO scan all AO shaders
//
	for (size_t i=0;i<1/*m_AOShaders.size()*/;i++)
	{
		CAmbientOcclusionShader* ambientOcclusion = m_AOShaders[i];

		occluders.clear();
		transforms.clear();

		for (size_t j=0;j<m_AOShaders.size();j++)
		{
			if (j != i)
			{
				occluders.push_back(m_AOShaders[j]);
				CGenericMatrix<float> T;
				const CObject3DInstance *transform = m_AOShaders[j]->getTransformSource();
				if (transform == NULL)
					T.Ident();
				else
					transform->getTransform(T);
				transforms.push_back(T);
			}
		}
		ambientOcclusion->setOccluders(occluders,transforms);
		ambientOcclusion->glRender();
	}

	// Third step: render objects using the AO map
	glRenderObjects(objects, C3DScene::FULL_PASS);
}


void CAmbientOcclusion::glRenderTexture(void)
{
	glEnable(GL_TEXTURE_2D);
	m_AOShaders[0]->glRenderResult();
}

