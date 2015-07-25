// BumppedGeometry.cpp: implementation of the CBumppedGeometry class.
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
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif


RAPTOR_NAMESPACE

CEMBMGeometry::CEMBMGeometry(const std::string& name)
	:CBumppedGeometry(name)
{
}


CEMBMGeometry::~CEMBMGeometry()
{
}


void CEMBMGeometry::glRender()
{
	if (!properties.isVisible())
		return;

#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if (diffuseMap == NULL)
	{
		return;
		Raptor::GetErrorManager()->generateRaptorError(CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
			CRaptorErrorManager::RAPTOR_WARNING,
			CRaptorMessages::ID_TEXTURE_MISS);
	}
#endif

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
		m_pBumpShader->glRender();
		m_pBumpShader->glRenderTexture();

		CVertexShader *vs = m_pBumpShader->glGetVertexShader();
		vs->glProgramParameter(10, X);
		vs->glProgramParameter(11, V);
		vs->glProgramParameter(12, A);

		CGeometry::glRenderGeometry();

		const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glActiveTextureARB(GL_TEXTURE3_ARB);
#if defined(GL_ARB_texture_cube_map)
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glDisable(GL_TEXTURE_2D);
#endif
		pExtensions->glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);

		m_pBumpShader->glStop();
	}

	CATCH_GL_ERROR
}

void CEMBMGeometry::setRenderingModel(const CRenderingModel& model)
{
	const CGeometryEditor &pEditor = getEditor();
	if (normals == NULL)
		pEditor.genNormals(true);
	else if (tangents == NULL)
		pEditor.genNormals(false);
	pEditor.genBinormals();

	//  render material normals is mandatory for this kind of geometry
	CRenderingModel l_model = model;
	l_model.addModel(CRenderingModel::CGL_NORMALS);
	l_model.addModel(CRenderingModel::CGL_TANGENTS);
	CGeometry::setRenderingModel(l_model);

	CVertexShader *vs = m_pBumpShader->glGetVertexShader("EMBM_VTX_SHADER");
	vs = m_pBumpShaderAmbient->glGetVertexShader("EMBM_0LIGHT_VTX_SHADER");

#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if (!getRenderingModel().hasModel(CRenderingModel::CGL_SHADER_EMBM))
		Raptor::GetErrorManager()->generateRaptorError(	CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_WRONG_RENDERING);
#endif

	if ((normalMap == NULL) || (diffuseMap == NULL) || (envMap == NULL))
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
			CRaptorErrorManager::RAPTOR_WARNING,
			CRaptorMessages::ID_TEXTURE_MISS);
#endif
		return;
	}

	CTextureUnitSetup *setup = m_pBumpShader->glGetTextureUnitsSetup();
	setup->setDiffuseMap(diffuseMap);
	setup->setNormalMap(normalMap);
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

	CATCH_GL_ERROR
}

