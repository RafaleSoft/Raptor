/***************************************************************************/
/*                                                                         */
/*  Shader.cpp                                                             */
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


#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "Shader.h"
#endif
#if !defined(AFX_MATERIAL_H__B42ABB88_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "Material.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "TextureFactory.h"
#endif
#if !defined(AFX_TEXTUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "TextureUnitSetup.h"
#endif
#if !defined(AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_)
	#include "Subsys/ShaderLibrary.h"
#endif
#if !defined(AFX_VULKANSHADERSTAGE_H__EF5769B8_470D_467F_9FDE_553142C81698__INCLUDED_)
	#include "VulkanShaderStage.h"
#endif
#if !defined(AFX_OPENGLPROGRAMSTAGE_H__0BCE3B42_6E10_4F50_BB27_1993345ADBCF__INCLUDED_)
	#include "OpenGLProgramStage.h"
#endif
#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "OpenGLShaderStage.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "ObjectFactory.h"
#endif
#if !defined(AFX_IRENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "IRenderingProperties.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif


//////////////////////////////////////////////////////////////////////
// Static data
//////////////////////////////////////////////////////////////////////


RAPTOR_NAMESPACE_BEGIN

//	Static datas
//  By default, shader applyies default global ambient.
CColor::RGBA	CShader::m_ambient = CColor::RGBA(0.2f,0.2f,0.2f,1.0f);

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

IMPLEMENT_CLASS_ID(CShader, shaderID)


CColor::RGBA CShader::getAmbient(void) 
{ 
    return m_ambient; 
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CShader::CShader(const std::string& name)
	:CPersistence(shaderID, name), CObjectReference(),
	m_pTMUSetup(NULL),m_pMaterial(NULL),
	m_pOpenGLProgram(NULL), m_pOpenGLShader(NULL), m_pVulkanShader(NULL)
{
	m_textureUnitSetup.handle(0);
	m_textureUnitSetup.hClass(CTextureUnitSetup::CTextureUnitSetupClassID::GetClassId().ID());
	m_textureUnitUnSetup.handle(0);
	m_textureUnitUnSetup.hClass(CTextureUnitSetup::CTextureUnitSetupClassID::GetClassId().ID());

	m_color.r = 0.0f;
	m_color.g = 0.0f;
	m_color.b = 0.0f;
	m_color.a = 1.0f;

	m_bDeleteVulkanShader = false;
	m_bDeleteOpenGLProgram = false;
	m_bDeleteOpenGLShader = false;
	m_bDeleteTMUSetup = false;
}

CShader::CShader(const CShader& shader)
	:CPersistence(shaderID, shader.getName()), CObjectReference(),
	m_pTMUSetup(NULL), m_pMaterial(NULL),
	m_pOpenGLProgram(NULL), m_pOpenGLShader(NULL), m_pVulkanShader(NULL)
{
	m_color = shader.m_color;
	m_ambient = shader.m_ambient;
	
	m_textureUnitSetup.handle(0); // glBuildSetup done at first call to glRender
	m_textureUnitSetup.hClass(CTextureUnitSetup::CTextureUnitSetupClassID::GetClassId().ID());
	m_textureUnitUnSetup.handle(0);
	m_textureUnitUnSetup.hClass(CTextureUnitSetup::CTextureUnitSetupClassID::GetClassId().ID());

	if (NULL != shader.m_pMaterial)
	{
		glRemoveMaterial();
		m_pMaterial = new CMaterial(*shader.m_pMaterial);
		m_pMaterial->registerDestruction(this);
	}
	if (NULL != shader.m_pTMUSetup)
	{
		glRemoveTextureUnitSetup();
		if (shader.m_bDeleteTMUSetup)
			m_pTMUSetup = new CTextureUnitSetup(*shader.m_pTMUSetup);
		else
			m_pTMUSetup = shader.m_pTMUSetup;
		m_pTMUSetup->registerDestruction(this);
	}
	if (NULL != shader.m_pVulkanShader)
	{
		vkRemoveVulkanShader();
		if (shader.m_bDeleteVulkanShader)
			m_pVulkanShader = shader.m_pVulkanShader->vkClone();
		else
			m_pVulkanShader = shader.m_pVulkanShader;
		m_pVulkanShader->registerDestruction(this);
	}
	if (NULL != shader.m_pOpenGLProgram)
	{
		glRemoveOpenGLProgram();
		if (shader.m_bDeleteOpenGLProgram)
			m_pOpenGLProgram = shader.m_pOpenGLProgram->glClone();
		else
			m_pOpenGLProgram = shader.m_pOpenGLProgram;
		m_pOpenGLProgram->registerDestruction(this);
	}
	if (NULL != shader.m_pOpenGLShader)
	{
		glRemoveOpenGLShader();
		if (shader.m_bDeleteOpenGLShader)
			m_pOpenGLShader = shader.m_pOpenGLShader->glClone();
		else
			m_pOpenGLShader = shader.m_pOpenGLShader;
		m_pOpenGLShader->registerDestruction(this);
	}

	m_bDeleteVulkanShader = shader.m_bDeleteVulkanShader;
	m_bDeleteOpenGLProgram = shader.m_bDeleteOpenGLProgram;
	m_bDeleteOpenGLShader = shader.m_bDeleteOpenGLShader;
	m_bDeleteTMUSetup = shader.m_bDeleteTMUSetup;
}

CShader* CShader::glClone(const std::string& newShaderName) const
{
	CShader* new_shader = new CShader(*this);

	CATCH_GL_ERROR

	if (!newShaderName.empty())
		new_shader->setName(newShaderName);
	else
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(	CShader::CShaderClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														"Attempting to clone a shader with no final name");
#endif
		new_shader->setName(getName());
	}

	return new_shader;
}

const CShader& CShader::getShader(const std::string& shaderName)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();

	CPersistence *ppShader = NULL;
	if (!shaderName.empty())
		ppShader = CPersistence::FindObject(shaderName);
	if (ppShader == NULL)
		return instance.getNullShader();
	else if (ppShader->getId().isSubClassOf(CShader::CShaderClassID::GetClassId()))
		return *(CShader*)ppShader;
	else
		return instance.getNullShader();
}

CShader::~CShader()
{
	glRemoveMaterial();
	glRemoveTextureUnitSetup();
	glRemoveOpenGLProgram();
	glRemoveOpenGLShader();
	vkRemoveVulkanShader();
}

void CShader::glRender()
{
	if (NULL != m_pOpenGLShader)
		m_pOpenGLShader->glRender();
	else if (NULL != m_pOpenGLProgram)
		m_pOpenGLProgram->glRender();

	CATCH_GL_ERROR
}

void CShader::glStop()
{
	if (NULL != m_pOpenGLShader)
		m_pOpenGLShader->glStop();
	else if (NULL != m_pOpenGLProgram)
		m_pOpenGLProgram->glStop();

	//	This call generates too much load in GL client/server.
	//	Derived classes shall implement own UnSetup until a more general
	//	TMU resource allocation manager can be used
	/*
	if ((0 != m_textureUnitSetup.handle) &&
		(0 != m_textureUnitUnSetup.handle))
		if (IRenderingProperties::GetCurrentProperties()->getCurrentTexturing() == IRenderingProperties::ENABLE)
			glCallList(m_textureUnitUnSetup.handle);
	*/
}

void CShader::setAmbient(GLfloat r,GLfloat g,GLfloat b,GLfloat a) 
{ 
    m_ambient.r = r; 
    m_ambient.g = g; 
    m_ambient.b = b; 
    m_ambient.a = a; 
}

void CShader::unLink(const CPersistence* p)
{
    if (p == static_cast<CPersistence*>(m_pMaterial))
        m_pMaterial = NULL;
    else if (p == static_cast<CPersistence*>(m_pTMUSetup))
        m_pTMUSetup = NULL;
	else if (p == static_cast<CPersistence*>(m_pVulkanShader))
		m_pVulkanShader = NULL;
	else if (p == static_cast<CPersistence*>(m_pOpenGLProgram))
		m_pOpenGLProgram = NULL;
	else if (p == static_cast<CPersistence*>(m_pOpenGLShader))
		m_pOpenGLShader = NULL;
}

CMaterial * const CShader::getMaterial(void)
{ 
	if (m_pMaterial == NULL)
    {
		m_pMaterial = new CMaterial(CMaterial::CGL_DARKGRAY_MATERIAL,
									CMaterial::CGL_GRAY_MATERIAL,
									CMaterial::CGL_LIGHTGRAY_MATERIAL,
									4.0f,
									CMaterial::CGL_NO_MATERIAL,
									getName() + " Material");
        m_pMaterial->registerDestruction(this);
    }

	return m_pMaterial;
}

bool CShader::glRemoveMaterial(void)
{
    if (m_pMaterial == NULL)
        return false;
    else
    {
        m_pMaterial->unregisterDestruction(this);
        delete m_pMaterial;
        m_pMaterial = NULL;

        CATCH_GL_ERROR

        return true;
    }
}

CTextureUnitSetup * const CShader::glGetTextureUnitsSetup(const std::string& name)
{
	if (m_pTMUSetup == NULL)
    {
		CPersistence *pTMU = NULL;
		if (!name.empty())
			pTMU = CPersistence::FindObject(name);

		if (pTMU == NULL)
		{
			m_pTMUSetup = new CTextureUnitSetup();
			m_bDeleteTMUSetup = true;
		}
		else if (pTMU->getId().isSubClassOf(CTextureUnitSetup::CTextureUnitSetupClassID::GetClassId()))
			m_pTMUSetup = (CTextureUnitSetup*)pTMU;

		m_pTMUSetup->registerDestruction(this);

		CATCH_GL_ERROR
    }

	return m_pTMUSetup;
}

bool CShader::glRemoveTextureUnitSetup(void)
{
    if (m_pTMUSetup == NULL)
        return false;
    else
    {
		m_pTMUSetup->unregisterDestruction(this);

		if (m_bDeleteTMUSetup)
			delete m_pTMUSetup;	

        m_pTMUSetup = NULL;
		m_bDeleteTMUSetup = false;

		if (m_textureUnitSetup.glname() != 0)
		{
			glDeleteLists(m_textureUnitSetup.glname(), 1);
			m_textureUnitSetup.handle(0);
		}
		if (m_textureUnitUnSetup.glname() != 0)
		{
			glDeleteLists(m_textureUnitUnSetup.glname(), 1);
			m_textureUnitUnSetup.handle(0);
		}

        CATCH_GL_ERROR

        return true;
    }
}

COpenGLProgramStage * const CShader::glGetOpenGLProgram(const std::string& name)
{
	if (m_pOpenGLProgram == NULL)
	{
		CPersistence *pProgram = NULL;
		if (!name.empty())
			pProgram = CPersistence::FindObject(name);
		if (pProgram == NULL)
		{
			m_pOpenGLProgram = new COpenGLProgramStage(name);
			m_bDeleteOpenGLProgram = true;
		}
		else if (pProgram->getId().isSubClassOf(COpenGLProgramStage::COpenGLProgramStageClassID::GetClassId()))
			m_pOpenGLProgram = (COpenGLProgramStage*)pProgram;

		m_pOpenGLProgram->registerDestruction(this);

		CATCH_GL_ERROR
	}

	return m_pOpenGLProgram;
}

bool CShader::glRemoveOpenGLProgram(void)
{
	if (m_pOpenGLProgram == NULL)
		return false;
	else
	{
		m_pOpenGLProgram->unregisterDestruction(this);
		// TODO : ressource linked to pipeline

		if (m_bDeleteOpenGLProgram)
			delete m_pOpenGLProgram;
		m_pOpenGLProgram = NULL;
		m_bDeleteOpenGLProgram = false;

		CATCH_GL_ERROR

		return true;
	}
}

COpenGLShaderStage * const CShader::glGetOpenGLShader(const std::string& name)
{
	if (m_pOpenGLShader == NULL)
	{
		CPersistence *pShader = NULL;
		if (!name.empty())
			pShader = CPersistence::FindObject(name);
		if (pShader == NULL)
		{
			m_pOpenGLShader = new COpenGLShaderStage(name);
			m_bDeleteOpenGLShader = true;
		}
		else if (pShader->getId().isSubClassOf(COpenGLShaderStage::COpenGLShaderStageClassID::GetClassId()))
			m_pOpenGLShader = (COpenGLShaderStage*)pShader;

		m_pOpenGLShader->registerDestruction(this);

		CATCH_GL_ERROR
	}

	return m_pOpenGLShader;
}

bool CShader::glRemoveOpenGLShader(void)
{
	if (m_pOpenGLShader == NULL)
		return false;
	else
	{
		m_pOpenGLShader->unregisterDestruction(this);
		// TODO : ressource linked to pipeline

		if (m_bDeleteOpenGLShader)
			delete m_pOpenGLShader;
		m_pOpenGLShader = NULL;
		m_bDeleteOpenGLShader = false;

		CATCH_GL_ERROR

		return true;
	}
}

CVulkanShaderStage * const CShader::vkGetVulkanShader(const std::string& name)
{
	if (m_pVulkanShader == NULL)
	{
		CPersistence *pProgram = NULL;
		if (!name.empty())
			pProgram = CPersistence::FindObject(name);
        if (pProgram == NULL)
		{
			m_pVulkanShader = new CVulkanShaderStage(name);
			m_bDeleteVulkanShader = true;
		}
		else if (pProgram->getId().isSubClassOf(CVulkanShaderStage::CVulkanShaderStageClassID::GetClassId()))
			m_pVulkanShader = (CVulkanShaderStage*)pProgram;

		m_pVulkanShader->registerDestruction(this);

        CATCH_GL_ERROR
	}

	return m_pVulkanShader;
}

bool CShader::vkRemoveVulkanShader(void)
{
	if (m_pVulkanShader == NULL)
		return false;
	else
	{
		m_pVulkanShader->unregisterDestruction(this);
		// TODO : ressource linked to pipeline

		if (m_bDeleteVulkanShader)
			delete m_pVulkanShader;
		m_pVulkanShader = NULL;
		m_bDeleteVulkanShader = false;

		CATCH_GL_ERROR

		return true;
	}
}

void CShader::glRenderMaterial(void)
{
	if ((NULL != m_pMaterial) && (NULL != m_pOpenGLShader))
	{
		if (m_pMaterial->doRebuild())
		{
			CProgramParameters::CParameter<CMaterial::Material_t> material(m_pMaterial->getMaterial());
			material.name("Material");
			material.locationType = GL_UNIFORM_BLOCK_BINDING_ARB;
			CProgramParameters params;
			params.addParameter(material);

			m_pOpenGLShader->updateProgramParameters(params);
		}
	}

	if (IRenderingProperties::GetCurrentProperties()->getCurrentLighting() == IRenderingProperties::ENABLE)
	{
		if (m_pMaterial != NULL)
		    m_pMaterial->glRender();
		else
			glColor4f(m_color.r, m_color.g, m_color.b, m_color.a);
	}
	else
    {
        if (m_pMaterial != NULL)
        {
            //  Apply a color proportional to global ambient and material ambient settings
            CColor::RGBA ambientMat = m_pMaterial->getAmbient();
            glColor4f(m_ambient.r * ambientMat.r,m_ambient.g * ambientMat.g,m_ambient.b * ambientMat.b,1.0f);
        }
        else
		    glColor4f(m_color.r, m_color.g, m_color.b, m_color.a);
    }

    CATCH_GL_ERROR
}


void CShader::glRenderTexture(void)
{
	if (IRenderingProperties::GetCurrentProperties()->getCurrentTexturing() == IRenderingProperties::ENABLE)
	{
		if (m_textureUnitSetup.handle() > 0)
        {
			glCallList(m_textureUnitSetup.glname());
        }
		else if (m_pTMUSetup != NULL)
		{
			m_textureUnitUnSetup = m_pTMUSetup->glBuildUnSetup();
			m_textureUnitSetup = m_pTMUSetup->glBuildSetup();
			glCallList(m_textureUnitSetup.glname());
		}
	}

	CATCH_GL_ERROR
}


bool CShader::exportObject(CRaptorIO& o)
{
	CPersistence::exportObject(o);
	return true;
}

bool CShader::importObject(CRaptorIO& io)
{
    string name;
    io >> name;

	string data = io.getValueName();
    while (!data.empty())
    {
		if (data == "name")
			CPersistence::importObject(io);
        else if (data =="Color")
            io >> m_color;
		else if (data == "Material")
		{
			getMaterial();
			m_pMaterial->importObject(io);
		}
		else if (data == "TMUSetup")
		{
			glGetTextureUnitsSetup();
			m_pTMUSetup->importObject(io);
		}
		else if (data == "VulkanShader")
		{
			vkGetVulkanShader();
			m_pVulkanShader->importObject(io);
		}
		else
			io >> name;
		
		data = io.getValueName();
	}
	io >> name;

	return true;
}
