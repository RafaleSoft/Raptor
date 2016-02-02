// Shader.cpp: implementation of the CShader class.
//
//////////////////////////////////////////////////////////////////////
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
#if !defined(AFX_VERTEXSHADER_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)
	#include "VertexShader.h"
#endif
#if !defined(AFX_VERTEXPROGRAM_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
    #include "VertexProgram.h"
#endif
#if !defined(AFX_FRAGMENTSHADER_H__66B3089A_2919_4678_9273_6CDEF7E5787F__INCLUDED_)
	#include "FragmentShader.h"
#endif
#if !defined(AFX_FRAGMENTPROGRAM_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
    #include "FragmentProgram.h"
#endif
#if !defined(AFX_GEOMETRYPROGRAM_H__1981EA98_8F3C_4881_9429_A9ACA5B285D3__INCLUDED_)
    #include "GeometryProgram.h"
#endif
#if !defined(AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_)
	#include "Subsys/ShaderLibrary.h"
#endif
#if !defined(AFX_PROJECTOR_H__0AEE2092_215F_40FA_BBAE_7D8A2F5A482F__INCLUDED_)
    #include "Projector.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "ObjectFactory.h"
#endif
#if !defined(AFX_RENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "RenderingProperties.h"
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

CColor::RGBA CShader::getAmbient(void) 
{ 
    return m_ambient; 
}

static CShader::CShaderClassID shaderID;
static CPersistentType<CShader> shaderFactory(shaderID);
const CPersistence::CPersistenceClassID& CShader::CShaderClassID::GetClassId(void)
{
	return shaderID;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CShader::CShader(const std::string& name)
	:CPersistence(shaderID, name), CObjectReference(),
	m_pTMUSetup(NULL),m_pMaterial(NULL),
    m_pVShader(NULL),m_pFShader(NULL),
    m_pVProgram(NULL),m_pFProgram(NULL),
	m_pGProgram(NULL)
{
	m_textureUnitSetup.handle = 0;
	m_textureUnitSetup.hClass = 0;
	m_textureUnitUnSetup.handle = 0;
	m_textureUnitUnSetup.hClass = 0;
    m_shaderProgram.handle = 0;
	m_shaderProgram.hClass = CShader::CShaderClassID::GetClassId().ID();

	m_color.r = 0.0f;
	m_color.g = 0.0f;
	m_color.b = 0.0f;
	m_color.a = 1.0f;

	m_bDeleteFShader = false;
	m_bDeleteVShader = false;
    m_bDeleteFProgram = false;
	m_bDeleteVProgram = false;
	m_bDeleteGProgram = false;
	m_bDeleteTMUSetup = false;
}

CShader::CShader(const CShader& shader)
	:CPersistence(shaderID, shader.getName()), CObjectReference(),
	m_pTMUSetup(NULL), m_pMaterial(NULL),
	m_pVShader(NULL), m_pFShader(NULL),
	m_pVProgram(NULL), m_pFProgram(NULL),
	m_pGProgram(NULL)
{
	m_color = shader.m_color;
	m_ambient = shader.m_ambient;
	
	m_shaderProgram.handle = 0;
	m_shaderProgram.hClass = CShader::CShaderClassID::GetClassId().ID();
	m_textureUnitSetup.handle = 0; // glBuildSetup done at first call to glRender
	m_textureUnitSetup.hClass = 0;
	m_textureUnitUnSetup.handle = 0;
	m_textureUnitUnSetup.hClass = 0;

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
		{
			m_pTMUSetup = new CTextureUnitSetup(*shader.m_pTMUSetup);
			m_pTMUSetup->registerDestruction(this);
		}
		else
			m_pTMUSetup = shader.m_pTMUSetup;
	}
	if (NULL != shader.m_pVShader)
	{
		glRemoveVertexShader();
		if (shader.m_bDeleteVShader)
		{
			m_pVShader = shader.m_pVShader->glClone();
			m_pVShader->registerDestruction(this);
		}
		else
			m_pVShader = shader.m_pVShader;
	}
	if (NULL != shader.m_pFShader)
	{
		glRemoveFragmentShader();
		if (shader.m_bDeleteFShader)
		{
			m_pFShader = shader.m_pFShader->glClone();
			m_pFShader->registerDestruction(this);
		}
		else
			m_pFShader = shader.m_pFShader;
	}
	if (NULL != shader.m_pVProgram)
	{
		glRemoveVertexProgram();
		if (shader.m_bDeleteVProgram)
		{
			m_pVProgram = shader.m_pVProgram->glClone();
			m_pVProgram->registerDestruction(this);
		}
		else
			m_pVProgram = shader.m_pVProgram;
	}
	if (NULL != shader.m_pFProgram)
	{
		glRemoveFragmentProgram();
		if (shader.m_bDeleteFProgram)
		{
			m_pFProgram = shader.m_pFProgram->glClone();
			m_pFProgram->registerDestruction(this);
		}
		else
			m_pFProgram = shader.m_pFProgram;
	}
	if (NULL != shader.m_pGProgram)
	{
		glRemoveGeometryProgram();
		if (shader.m_bDeleteGProgram)
		{
			m_pGProgram = shader.m_pGProgram->glClone();
			m_pGProgram->registerDestruction(this);
		}
		else
			m_pGProgram = shader.m_pGProgram;
	}

	if (0 != shader.m_shaderProgram.handle)
		glCompileShader();

	m_bDeleteVShader = shader.m_bDeleteVShader;
	m_bDeleteFShader = shader.m_bDeleteFShader;
	m_bDeleteVProgram = shader.m_bDeleteVProgram;
	m_bDeleteFProgram = shader.m_bDeleteFProgram;
	m_bDeleteGProgram = shader.m_bDeleteGProgram;
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
														"Attempting to clone a shader with no final name")
#endif
		new_shader->setName(getName());
	}

	return new_shader;
}

const CShader& CShader::getShader(const std::string& shaderName)
{
	CShaderLibrary *lib = CShaderLibrary::GetInstance();
	lib->glInitFactory();

	CPersistence *ppShader = NULL;
	if (!shaderName.empty())
		ppShader = CPersistence::FindObject(shaderName);
	if (ppShader == NULL)
		return CShaderLibrary::GetInstance()->getNullShader();
	else if (ppShader->getId().isSubClassOf(CShader::CShaderClassID::GetClassId()))
		return *(CShader*)ppShader;
	else
		return CShaderLibrary::GetInstance()->getNullShader();
}

CShader::~CShader()
{
	// TODO : delete program only if not shared !!!
#if defined(GL_ARB_shader_objects)
    if ((m_shaderProgram.handle != 0) &&
		(m_bDeleteVProgram || m_bDeleteFProgram || m_bDeleteGProgram))
    {
        const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

        GLsizei maxCount = 0;
        pExtensions->glGetObjectParameterivARB(m_shaderProgram.handle,GL_OBJECT_ATTACHED_OBJECTS_ARB,&maxCount);

        GLsizei count = 0;
        GLhandleARB *pHandles = new GLhandleARB[maxCount];
        pExtensions->glGetAttachedObjectsARB(m_shaderProgram.handle, maxCount,&count, pHandles);

        for (GLsizei i=0;i<count;i++)
            pExtensions->glDetachObjectARB(m_shaderProgram.handle, pHandles[i]);

        pExtensions->glDeleteObjectARB(m_shaderProgram.handle);
        delete [] pHandles;

		CShaderLibrary::GetInstance()->unRegisterProgram(m_shaderProgram);
    }
#endif

	glRemoveMaterial();
	glRemoveTextureUnitSetup();
	glRemoveVertexShader();
	glRemoveFragmentShader();
	glRemoveVertexProgram();
	glRemoveFragmentProgram();
	glRemoveGeometryProgram();
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
    else if (p == static_cast<CPersistence*>(m_pVShader))
        m_pVShader = NULL;
    else if (p == static_cast<CPersistence*>(m_pFShader))
        m_pFShader = NULL;
    else if (p == static_cast<CPersistence*>(m_pVProgram))
        m_pVProgram = NULL;
    else if (p == static_cast<CPersistence*>(m_pFProgram))
        m_pFProgram = NULL;
	else if (p == static_cast<CPersistence*>(m_pGProgram))
        m_pGProgram = NULL;
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

		if (m_textureUnitSetup.handle != 0)
		{
			glDeleteLists(m_textureUnitSetup.handle, 1);
			m_textureUnitSetup.handle = 0;
			m_textureUnitSetup.hClass = 0;
		}

        CATCH_GL_ERROR

        return true;
    }
}

CVertexProgram* const CShader::glGetVertexProgram(const std::string& name)
{
	if (m_pVProgram == NULL) 
	{
		CShaderLibrary *lib = CShaderLibrary::GetInstance();
		lib->glInitFactory();

		CPersistence *pProgram = NULL;
		if (!name.empty())
			pProgram = CPersistence::FindObject(name);
        if (pProgram == NULL)
		{
			m_pVProgram = new CVertexProgram(name);
			m_bDeleteVProgram = true;
		}
		else if (pProgram->getId().isSubClassOf(CVertexProgram::CVertexProgramClassID::GetClassId()))
			m_pVProgram = (CVertexProgram*)pProgram;

		m_pVProgram->registerDestruction(this);

        CATCH_GL_ERROR
	}

	return m_pVProgram;
}

bool CShader::glRemoveVertexProgram(void)
{
	if (m_pVProgram == NULL)
		return false;
	else
	{
		m_pVProgram->unregisterDestruction(this);

		if (m_bDeleteVProgram)
		{
			RAPTOR_HANDLE handle(0, (void*)m_shaderProgram.handle);
			m_pVProgram->glUnbindProgram(handle);
			delete m_pVProgram;
		}

		m_pVProgram = NULL;
		m_bDeleteVProgram = false;

		CATCH_GL_ERROR

		return true;
	}
}

CVertexShader * const CShader::glGetVertexShader(const std::string& name)
{
	if (m_pVShader == NULL) 
	{
		CShaderLibrary *lib = CShaderLibrary::GetInstance();
		lib->glInitFactory();

		CPersistence *pShader = NULL;
		if (!name.empty())
			pShader = CPersistence::FindObject(name);
		if (pShader == NULL)
		{
			m_pVShader = new CVertexShader(name);
			m_bDeleteVShader = true;
		}
		else if (pShader->getId().isSubClassOf(CVertexShader::CVertexShaderClassID::GetClassId()))
			m_pVShader = (CVertexShader*)pShader;

		m_pVShader->registerDestruction(this);

        CATCH_GL_ERROR
	}

	return m_pVShader;
}

bool CShader::glRemoveVertexShader(void)
{
	if (m_pVShader == NULL)
		return false;
	else
	{
		m_pVShader->unregisterDestruction(this);

		if (m_bDeleteVShader)
			delete m_pVShader;
		m_pVShader = NULL;
		m_bDeleteVShader = false;

		CATCH_GL_ERROR

		return true;
	}
}

CFragmentProgram * const CShader::glGetFragmentProgram(const std::string& name)
{
	if (m_pFProgram == NULL) 
	{
		CShaderLibrary *lib = CShaderLibrary::GetInstance();
		lib->glInitFactory();

		CPersistence *pProgram = NULL;
		if (!name.empty())
			pProgram = CPersistence::FindObject(name);
		if (pProgram == NULL)
		{
			m_pFProgram = new CFragmentProgram(name);
			m_bDeleteFProgram = true;
		}
		else if	(pProgram->getId().isSubClassOf(CFragmentProgram::CFragmentProgramClassID::GetClassId()))
			m_pFProgram = (CFragmentProgram*)pProgram;

		m_pFProgram->registerDestruction(this);

        CATCH_GL_ERROR
	}

	return m_pFProgram;
}

bool CShader::glRemoveFragmentProgram(void)
{
	if (m_pFProgram == NULL)
		return false;
	else
	{
		m_pFProgram->unregisterDestruction(this);

		if (m_bDeleteFProgram)
		{
			RAPTOR_HANDLE handle(0, (void*)m_shaderProgram.handle);
			m_pFProgram->glUnbindProgram(handle);
			delete m_pFProgram;
		}

		m_pFProgram = NULL;
		m_bDeleteFProgram = false;

		CATCH_GL_ERROR

		return true;
	}
}

CFragmentShader * const CShader::glGetFragmentShader(const std::string& name)
{
	if (m_pFShader == NULL) 
	{
		CShaderLibrary *lib = CShaderLibrary::GetInstance();
		lib->glInitFactory();

		CPersistence *pShader = NULL;
		if (!name.empty())
			pShader = CPersistence::FindObject(name);
		if (pShader == NULL)
		{
			m_pFShader = new CFragmentShader(name);
			m_bDeleteFShader = true;
		}
		else if	(pShader->getId().isSubClassOf(CFragmentShader::CFragmentShaderClassID::GetClassId()))
			m_pFShader = (CFragmentShader*)pShader;

		m_pFShader->registerDestruction(this);

        CATCH_GL_ERROR
	}

	return m_pFShader;
}

bool CShader::glRemoveFragmentShader(void)
{
	if (m_pFShader == NULL)
		return false;
	else
	{
		m_pFShader->unregisterDestruction(this);

		if (m_bDeleteFShader)
			delete m_pFShader;
		m_pFShader = NULL;
		m_bDeleteFShader = false;

		CATCH_GL_ERROR

		return true;
	}
}

CGeometryProgram* const CShader::glGetGeometryProgram(const std::string& name)
{
	if (m_pGProgram == NULL) 
	{
		CShaderLibrary *lib = CShaderLibrary::GetInstance();
		lib->glInitFactory();

		CPersistence *pProgram = NULL;
		if (!name.empty())
			pProgram = CPersistence::FindObject(name);
        if (pProgram == NULL)
		{
			m_pGProgram = new CGeometryProgram(name);
			m_bDeleteGProgram = true;
		}
		else if (pProgram->getId().isSubClassOf(CGeometryProgram::CGeometryProgramClassID::GetClassId()))
			m_pGProgram = (CGeometryProgram*)pProgram;

		m_pGProgram->registerDestruction(this);

        CATCH_GL_ERROR
	}

	return m_pGProgram;
}

bool CShader::glRemoveGeometryProgram(void)
{
	if (m_pGProgram == NULL)
		return false;
	else
	{
		m_pGProgram->unregisterDestruction(this);

		if (m_bDeleteGProgram)
		{
			RAPTOR_HANDLE handle(0, (void*)m_shaderProgram.handle);
			m_pGProgram->glUnbindProgram(handle);
			delete m_pGProgram;
		}

		m_pGProgram = NULL;
		m_bDeleteGProgram = false;

		CATCH_GL_ERROR

		return true;
	}
}

void CShader::glRenderMaterial(void)
{
	//if (glIsEnabled(GL_LIGHTING) == GL_TRUE)
    if (CRenderingProperties::GetCurrentProperties()->getCurrentLighting() == CRenderingProperties::ENABLE)
	{
		if (m_pMaterial!=NULL)
		    m_pMaterial->glRender();
		else
			glColor4fv(m_color);
	}
	else
    {
        if (m_pMaterial!=NULL)
        {
            //  Apply a color proportional to global ambient and material ambient settings
            CColor::RGBA ambientMat = m_pMaterial->getAmbient();
            glColor4f(m_ambient.r * ambientMat.r,m_ambient.g * ambientMat.g,m_ambient.b * ambientMat.b,1.0f);
        }
        else
		    glColor4fv(m_color);
    }

    CATCH_GL_ERROR
}


void CShader::glRenderTexture(void)
{
	//if (glIsEnabled(GL_TEXTURE_2D) == GL_TRUE)
    if (CRenderingProperties::GetCurrentProperties()->getCurrentTexturing() == CRenderingProperties::ENABLE)
	{
		if (m_textureUnitSetup.handle > 0)
        {
		    glCallList(m_textureUnitSetup.handle);
        }
		else if (m_pTMUSetup != NULL)
		{
			m_textureUnitUnSetup = m_pTMUSetup->glBuildUnSetup();
			m_textureUnitSetup = m_pTMUSetup->glBuildSetup();
			glCallList(m_textureUnitSetup.handle);
		}
	}

	CATCH_GL_ERROR
}

void CShader::glRender()
{
    if (m_shaderProgram.handle != 0)
    {
#if defined(GL_ARB_shader_objects)
        const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
        pExtensions->glUseProgramObjectARB(m_shaderProgram.handle);

        if (m_pVProgram != NULL)
            m_pVProgram->glRender();

		if (m_pGProgram != NULL)
			m_pGProgram->glRender();

        if (m_pFProgram != NULL)
            m_pFProgram->glRender();
#endif
    }
    else
    {
		if (m_pVShader != NULL)
			m_pVShader->glRender();
		if (m_pFShader != NULL)
			m_pFShader->glRender();
    }

    CATCH_GL_ERROR
}

void CShader::glStop()
{
    if (m_shaderProgram.handle != 0)
    {
#if defined(GL_ARB_shader_objects)
		const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glUseProgramObjectARB(0);
#endif
	}
	else
	{
		if (m_pVShader != NULL)
			m_pVShader->glStop();
		if (m_pFShader != NULL)
			m_pFShader->glStop();
	}

	//	This call generates too much load in GL client/server.
	//	Derived classes shall implement own UnSetup until a more general
	//	TMU resource allocation manager can be used
	/*
	if ((0 != m_textureUnitSetup.handle) &&
		(0 != m_textureUnitUnSetup.handle))
		if (CRenderingProperties::GetCurrentProperties()->getCurrentTexturing() == CRenderingProperties::ENABLE)
			glCallList(m_textureUnitUnSetup.handle);
	*/
}

bool CShader::glCompileShader()
{
    const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

	RAPTOR_HANDLE program = CShaderLibrary::GetInstance()->getRegisteredProgram(m_pVProgram,m_pFProgram,m_pGProgram);
	if (program.handle != 0)
	{
		m_shaderProgram = program;
		return true;
	}

    // First try to generate programs.
    // This step is mandatory and must succeed if there are programs.
    if ((m_pFProgram != NULL) || (m_pVProgram != NULL) || (m_pGProgram != NULL))
    {
#if defined(GL_ARB_shader_objects)
        bool abort = false;

        // create a program object
        if (m_shaderProgram.handle == 0)
        {
			m_shaderProgram.handle = pExtensions->glCreateProgramObjectARB();
            if (m_shaderProgram.handle == 0)
            {
                abort = true;
				Raptor::GetErrorManager()->generateRaptorError(CShader::CShaderClassID::GetClassId(),
                                                               CRaptorErrorManager::RAPTOR_ERROR,
                                                               CRaptorMessages::ID_CREATE_FAILED,CRaptorMessages::no_args);
            }
        }

        // Attach programs before linking.
		// Query attributes is done before linking (in glBindProgram)
		// to be able to bind attributes locations
        if ((!abort) && (m_pVProgram != NULL))
        {
            if ((!m_pVProgram->isValid()) || (!m_pVProgram->glBindProgram(m_shaderProgram)))
                abort = true;
        }

        if ((!abort) && (m_pFProgram != NULL))
        {
            if ((!m_pFProgram->isValid()) || (!m_pFProgram->glBindProgram(m_shaderProgram)))
                abort = true;
        }

		if ((!abort) && (m_pGProgram != NULL))
        {
            if ((!m_pGProgram->isValid()) || (!m_pGProgram->glBindProgram(m_shaderProgram)))
                abort = true;
        }

        // link
        if (!abort)
        {
            pExtensions->glLinkProgramARB(m_shaderProgram.handle);
            GLint linkStatus = GL_FALSE;
            pExtensions->glGetObjectParameterivARB(m_shaderProgram.handle,GL_OBJECT_LINK_STATUS_ARB,&linkStatus);
            if (linkStatus == GL_FALSE)
                abort = true;
        }

#ifdef RAPTOR_DEBUG_MODE_GENERATION
        if (!abort)
        {
            pExtensions->glValidateProgramARB(shaderProgram.handle);
            GLint validateStatus = GL_FALSE;
            pExtensions->glGetObjectParameterivARB(shaderProgram.handle,GL_OBJECT_VALIDATE_STATUS_ARB,&validateStatus);
            if (validateStatus == GL_FALSE)
                abort = true;
        }
#endif

        if ((abort) && (m_shaderProgram.handle != 0))
        {
            GLint maxLength = 0;
            GLint length = 0;
	        pExtensions->glGetObjectParameterivARB(m_shaderProgram.handle,GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
	        char *pInfoLog = (char*) malloc(maxLength * sizeof(char));
	        pExtensions->glGetInfoLogARB(m_shaderProgram.handle, maxLength, &length, pInfoLog);

            CRaptorMessages::MessageArgument arg;
            arg.arg_sz = pInfoLog;
            vector<CRaptorMessages::MessageArgument> args;
            args.push_back(arg);
			Raptor::GetErrorManager()->generateRaptorError(	CShader::CShaderClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_ERROR,
															CRaptorMessages::ID_PROGRAM_ERROR,
															args);
            free(pInfoLog);
        }

        CATCH_GL_ERROR

		if (!abort)
			CShaderLibrary::GetInstance()->registerProgram(m_pVProgram,m_pFProgram,m_pGProgram,m_shaderProgram);

        return !abort;
#endif
    }
    
    return false;
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
		else if (data == "VertexShader")
		{
			glGetVertexShader();
			m_pVShader->importObject(io);
		}
		else if (data == "FragmentShader")
		{
			glGetFragmentShader();
			m_pFShader->importObject(io);
		}
        else if (data == "VertexProgram")
		{
			glGetVertexProgram();
			m_pVProgram->importObject(io);
		}
		else if (data == "GeometryProgram")
		{
			glGetGeometryProgram();
			m_pGProgram->importObject(io);
		}
		else if (data == "FragmentProgram")
		{
			glGetFragmentProgram();
			m_pFProgram->importObject(io);
		}
		else
			io >> name;
		
		data = io.getValueName();
	}
	io >> name;

	return true;
}



