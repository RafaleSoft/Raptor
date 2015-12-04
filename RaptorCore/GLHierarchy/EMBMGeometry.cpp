// EMBMGeometry.cpp: implementation of the CEMBMGeometry class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_EMBMGEOMETRY_H__0270535E_F6CF_4C2B_9F52_3BDB34627468__INCLUDED_)
	#include "EMBMGeometry.h"
#endif
#if !defined(AFX_GEOMETRYEDITOR_H__2D77E428_ED3D_416B_8DE9_DABFD45A38A7__INCLUDED_)
	#include "GeometryEditor.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "Shader.h"
#endif
#if !defined(AFX_VERTEXSHADER_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)
	#include "VertexShader.h"
#endif
#if !defined(AFX_FRAGMENTSHADER_H__66B3089A_2919_4678_9273_6CDEF7E5787F__INCLUDED_)
	#include "FragmentShader.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_3DENGINEMATRIX_H__6CD1110E_1174_4f38_A452_30FB312022D0__INCLUDED_)
	#include "Engine/3DEngineMatrix.h"
#endif
#if !defined(AFX_SIMPLELIGHTOBSERVER_H__238FC166_A3BC_4D77_8FD4_0A42DB45280F__INCLUDED_)
	#include "Subsys/SimpleLightObserver.h"
#endif
#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "Light.h"
#endif


RAPTOR_NAMESPACE


CShader* CEMBMGeometry::m_pBumpShaderAmbient = NULL;
CSimpleLightObserver* CEMBMGeometry::m_pObserver = NULL;


CEMBMGeometry::CEMBMGeometry(const std::string& name)
	:CBumppedGeometry(name)
{
}

CEMBMGeometry::~CEMBMGeometry()
{
	if (NULL != m_pObserver)
	{
		bool lastObject = (m_pObserver->getRefCount() == 1);
		m_pObserver->releaseReference();
		if (lastObject)
			m_pObserver = NULL;
	}

	if (m_pBumpShaderAmbient != NULL)
	{
		m_pBumpShaderAmbient->unregisterDestruction(this);
		m_pBumpShaderAmbient->releaseReference();
	}
	/*
	if (m_pBumpShader2Lights != NULL)
	{
		m_pBumpShader2Lights->unregisterDestruction(this);
		m_pBumpShader2Lights->releaseReference();
	}
	if (m_pBumpShader3Lights != NULL)
	{
		m_pBumpShader3Lights->unregisterDestruction(this);
		m_pBumpShader3Lights->releaseReference();
	}
	*/
}


void CEMBMGeometry::unLink(const CPersistence* p)
{
	if (p == static_cast<CPersistence*>(m_pBumpShaderAmbient))
		m_pBumpShaderAmbient = NULL;
	/*
	else if (p == static_cast<CPersistence*>(m_pBumpShader2Lights))
		m_pBumpShader2Lights = NULL;
	else if (p == static_cast<CPersistence*>(m_pBumpShader3Lights))
		m_pBumpShader3Lights = NULL;
	*/
	else
		CBumppedGeometry::unLink(p);
}

void CEMBMGeometry::init(void)
{
	if (m_pObserver == NULL)
		m_pObserver = new CSimpleLightObserver();
	else
		m_pObserver->addReference();

	m_pBumpShader = new CShader("EMBM_SHADER");
	m_pBumpShader->registerDestruction(this);

	if (m_pBumpShaderAmbient == NULL)
		m_pBumpShaderAmbient = new CShader("EMBM_SHADER_AMBIENT");
	else
		m_pBumpShaderAmbient->addReference();
	m_pBumpShaderAmbient->registerDestruction(this);

	/*
	if (m_pBumpShader2Lights == NULL)
		m_pBumpShader2Lights = new CShader("BUMP_SHADER_2LIGHTS");
	else
		m_pBumpShader2Lights->addReference();
	m_pBumpShader2Lights->registerDestruction(this);

	if (m_pBumpShader3Lights == NULL)
		m_pBumpShader3Lights = new CShader("BUMP_SHADER_3LIGHTS");
	else
		m_pBumpShader3Lights->addReference();
	m_pBumpShader3Lights->registerDestruction(this);
	*/
}

unsigned int CEMBMGeometry::glUpdateLightPosition(void)
{
	unsigned int numLights = 0;
	C3DEngineMatrix T;

	glGetTransposeFloatv(GL_MODELVIEW_MATRIX, T);

	GL_COORD_VERTEX center;
	getCenter(center);
	CGenericVector<float> x(center.x, center.y, center.z, 1.0f);
	x *= T;
	vector<CLight*> lights = m_pObserver->sortLights(x);
	if (lights.size() < 1)
		return numLights;
	CLight *pMainLight = lights[0];

	V.x = -(T[0] * T[3] + T[4] * T[7] + T[8] * T[11]);
	V.y = -(T[1] * T[3] + T[5] * T[7] + T[9] * T[11]);
	V.z = -(T[2] * T[3] + T[6] * T[7] + T[10] * T[11]);

	T.Inverse();

	numLights++;
	X = T * pMainLight->getLightEyePosition();


	GL_COORD_VERTEX att = pMainLight->getSpotParams();
	A.x = att.z;
	A.y = att.y;
	A.z = att.x;
	A.h = 0.0f;
	S = pMainLight->getSpecular();

	return numLights;
}

void CEMBMGeometry::glRender()
{
	if (!properties.isVisible())
		return;

	unsigned int numLights = glUpdateLightPosition();

	m_pBumpShader->glRenderMaterial();

	if ((!glIsEnabled(GL_LIGHTING)) || (numLights == 0))
	{
		m_pBumpShaderAmbient->glRender();
		m_pBumpShader->glRenderTexture();

		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, sizeof(GL_COORD_VERTEX), normals);

		CGeometry::glRenderGeometry();

		glDisableClientState(GL_NORMAL_ARRAY);

		m_pBumpShaderAmbient->glStop();
	}
	else
	{	
		CFragmentShader *fs = NULL;
		CVertexShader *vs = NULL;

	//	if (numLights == 1)
		{
			m_pBumpShader->glRender();
			fs = m_pBumpShader->glGetFragmentShader();
			vs = m_pBumpShader->glGetVertexShader();
			vs->glProgramParameter(10, X);
			vs->glProgramParameter(11, V);
			//vs->glProgramParameter(12, A);
		}
		/*
		else if (numLights == 2)
		{
			fs = m_pBumpShader2Lights->glGetFragmentShader();
			fs->glRender();
			fs->glProgramParameter(0, S);
			fs->glProgramParameter(1, S2);

			vs = m_pBumpShader2Lights->glGetVertexShader();
			vs->glRender();
			vs->glProgramParameter(4, X);
			vs->glProgramParameter(5, V);
			vs->glProgramParameter(6, A);
			vs->glProgramParameter(7, X2);
			vs->glProgramParameter(8, A2);
		}
		else
		{
			fs = m_pBumpShader3Lights->glGetFragmentShader();
			fs->glRender();
			fs->glProgramParameter(0, S);
			fs->glProgramParameter(1, S2);
			fs->glProgramParameter(2, S3);

			vs = m_pBumpShader3Lights->glGetVertexShader();
			vs->glRender();
			vs->glProgramParameter(4, X);
			vs->glProgramParameter(5, V);
			vs->glProgramParameter(6, A);
			vs->glProgramParameter(7, X2);
			vs->glProgramParameter(8, A2);
			vs->glProgramParameter(9, X3);
			vs->glProgramParameter(10, A3);
		}*/

		m_pBumpShader->glRenderTexture();

		glRenderGeometry();

		vs->glStop();
		fs->glStop();

		const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glActiveTextureARB(GL_TEXTURE3_ARB);
#if defined(GL_ARB_texture_cube_map)
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glDisable(GL_TEXTURE_2D);
#endif
		pExtensions->glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
	}

	CATCH_GL_ERROR
}

void CEMBMGeometry::setRenderingModel(const CRenderingModel& model)
{
	CBumppedGeometry::setRenderingModel(model);

	CVertexShader *vs = m_pBumpShader->glGetVertexShader("EMBM_VTX_SHADER");
	vs = m_pBumpShaderAmbient->glGetVertexShader("EMBM_0LIGHT_VTX_SHADER");

	if (envMap == NULL)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(	CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_TEXTURE_MISS);
#endif
	}

	CTextureUnitSetup *setup = m_pBumpShader->glGetTextureUnitsSetup();
	setup->setEnvironmentMap(envMap);

#if defined (GL_ARB_fragment_program)
	setup->useRegisterCombiners(false);
	setup->getTMUShader(CTextureUnitSetup::IMAGE_UNIT_0).shaderOperation = GL_NONE;
	setup->getTMUShader(CTextureUnitSetup::IMAGE_UNIT_1).shaderOperation = GL_NONE;
	setup->getTMUShader(CTextureUnitSetup::IMAGE_UNIT_2).shaderOperation = GL_NONE;
	setup->getTMUShader(CTextureUnitSetup::IMAGE_UNIT_3).shaderOperation = GL_NONE;

	CFragmentShader *fs = m_pBumpShader->glGetFragmentShader("EMBM_TEX_SHADER");
	fs = m_pBumpShaderAmbient->glGetFragmentShader("EMBM_0LIGHT_TEX_SHADER");
#endif

	/*
#if defined (GL_ARB_fragment_program)
	setup->useRegisterCombiners(false);
	CVertexShader *vs = m_pBumpShader->glGetVertexShader("BUMP_ATT_VTX_SHADER");
	vs = m_pBumpShader2Lights->glGetVertexShader("BUMP_ATT_VTX_SHADER_2LIGHTS");
	vs = m_pBumpShaderAmbient->glGetVertexShader("BUMP_0LIGHT_VTX_SHADER");
	vs = m_pBumpShader3Lights->glGetVertexShader("BUMP_ATT_VTX_SHADER_3LIGHTS");

	CFragmentShader *fs = m_pBumpShader->glGetFragmentShader("BUMP_ATT_TEX_SHADER");
	fs = m_pBumpShader2Lights->glGetFragmentShader("BUMP_ATT_TEX_SHADER_2LIGHTS");
	fs = m_pBumpShaderAmbient->glGetFragmentShader("BUMP_0LIGHT_TEX_SHADER");
	fs = m_pBumpShader3Lights->glGetFragmentShader("BUMP_ATT_TEX_SHADER_3LIGHTS");
#endif
	*/

	if (!Raptor::glIsExtensionSupported("GL_ARB_texture_cube_map"))
	{
		Raptor::GetErrorManager()->generateRaptorError(	CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_NO_GPU_PROGRAM);
	}

	CATCH_GL_ERROR
}

