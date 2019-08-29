// BumppedGeometry.cpp: implementation of the CBumppedGeometry class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_BUMPPEDGEOMETRY_H__FDCE89B9_B923_4325_AB0D_A12486C6756D__INCLUDED_)
	#include "BumppedGeometry.h"
#endif
#if !defined(AFX_GEOMETRYEDITOR_H__2D77E428_ED3D_416B_8DE9_DABFD45A38A7__INCLUDED_)
    #include "GeometryEditor.h"
#endif
#if !defined(AFX_TEXTUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "TextureUnitSetup.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_VERTEXPROGRAM_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)
	#include "VertexProgram.h"
#endif
#if !defined(AFX_FRAGMENTPROGRAM_OLD_H__DD0AD51D_3BFF_4C65_8099_BA7696D7BDDF__INCLUDED_)
	#include "FragmentProgram_old.h"
#endif
#if !defined(AFX_EMBMSHADER_H__99A5AF45_D5C7_4F43_851C_A31FC52DB237__INCLUDED_)
	#include "Subsys/EMBMShader.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "ObjectFactory.h"
#endif
#if !defined(AFX_REFERENCE_H__D29BE5EA_DA55_4BCA_A700_73E007EFE5F9__INCLUDED_)
	#include "GLHierarchy/Reference.cxx"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_IRENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "IRenderingProperties.h"
#endif


RAPTOR_NAMESPACE

static CBumppedGeometry::CBumppedGeometryClassID bumpId;
static CPersistentObjectType<CBumppedGeometry> bumpFactory(bumpId);
const CPersistence::CPersistenceClassID& CBumppedGeometry::CBumppedGeometryClassID::GetClassId(void)
{
	return bumpId;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBumppedGeometry::CBumppedGeometry(const std::string& name)
	:CGeometry(name,bumpId),
	m_pBumpShader(NULL)
{
	init();
}

CBumppedGeometry::CBumppedGeometry(	const std::string& name, 
									const CPersistence::CPersistenceClassID &classID):
	CGeometry(name,classID),
	m_pBumpShader(NULL)
{
	init();
}

void CBumppedGeometry::init(void)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((!Raptor::glIsExtensionSupported(GL_ARB_FRAGMENT_PROGRAM_EXTENSION_NAME)) ||
		(!Raptor::glIsExtensionSupported(GL_ARB_VERTEX_PROGRAM_EXTENSION_NAME)) ||
		(!Raptor::glIsExtensionSupported(GL_ARB_FRAGMENT_SHADER_EXTENSION_NAME)) ||
		(!Raptor::glIsExtensionSupported(GL_ARB_VERTEX_SHADER_EXTENSION_NAME)))
	{
		Raptor::GetErrorManager()->generateRaptorError(	CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_NO_GPU_PROGRAM);
	}
#endif
	
	m_pBumpShader = (CEMBMShader*)CShader::getShader("EMBM_SHADER").glClone("BUMP_GEOMETRY_SHADER");
	m_pBumpShader->registerDestruction(this);

	CATCH_GL_ERROR
}


CBumppedGeometry::~CBumppedGeometry()
{
    diffuseMap = NULL;
	normalMap = NULL;
	envMap = NULL;
	
	if (m_pBumpShader != NULL)
	{
		m_pBumpShader->unregisterDestruction(this);
		m_pBumpShader->releaseReference();
	}
}

void CBumppedGeometry::unLink(const CPersistence* p)
{
	if (p == static_cast<CPersistence*>(m_pBumpShader))
		m_pBumpShader = NULL;
	else
		CGeometry::unLink(p);
}

CShader	* const CBumppedGeometry::getShader(void) const
{
	return m_pBumpShader;
}

void CBumppedGeometry::setDiffuseMap(CTextureObject* diffuse)
{
	// Check for null ?
	diffuseMap = diffuse;
	CTextureUnitSetup *setup = m_pBumpShader->glGetTextureUnitsSetup();
	setup->setDiffuseMap(diffuseMap);
}

void CBumppedGeometry::setNormalMap(CTextureObject* normal)
{
	normalMap = normal;
	CTextureUnitSetup *setup = m_pBumpShader->glGetTextureUnitsSetup();
	setup->setNormalMap(normalMap);
}

void CBumppedGeometry::setEnvironmentMap(CTextureObject* environment)
{
	envMap = environment;
	CTextureUnitSetup *setup = m_pBumpShader->glGetTextureUnitsSetup();
	setup->setEnvironmentMap(envMap);

	m_pBumpShader->enableEmbm(envMap != NULL);
}

void CBumppedGeometry::setRenderingModel(const CRenderingModel& model)
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
	
	if ((normalMap == NULL) || (diffuseMap == NULL))
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(	CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_TEXTURE_MISS);
		Raptor::GetErrorManager()->generateRaptorError(	CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														"Rendering will use Phong shading");
#endif
	}

	CTextureUnitSetup *setup = m_pBumpShader->glGetTextureUnitsSetup();
	setup->setDiffuseMap(diffuseMap);
	setup->setNormalMap(normalMap);
	setup->useRegisterCombiners(false);
	
	if (!Raptor::glIsExtensionSupported(GL_EXT_SECONDARY_COLOR_EXTENSION_NAME) ||
		!Raptor::glIsExtensionSupported(GL_ARB_FRAGMENT_PROGRAM_EXTENSION_NAME))
	{
		Raptor::GetErrorManager()->generateRaptorError(	CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_NO_GPU_PROGRAM);
	}

	CATCH_GL_ERROR
}


CBumppedGeometry& CBumppedGeometry::operator=(const CGeometry &geo)
{
	CGeometry::operator =(geo);

    const CGeometryEditor &pEditor = getEditor();
	pEditor.genBinormals();

	return *this;
}

CBumppedGeometry& CBumppedGeometry::operator=(const CBumppedGeometry &geo)
{
    CGeometry::operator =(geo);

	const CGeometryEditor &pEditor = getEditor();
	pEditor.genBinormals();

    return *this;
}

void CBumppedGeometry::glRender()
{
	if (!properties.isVisible())
		return;
    
	if (m_pBumpShader != NULL)
	{
		m_pBumpShader->glRenderMaterial();
		m_pBumpShader->glRenderTexture();
		m_pBumpShader->glRender();
	}
	
	glRenderGeometry();
	
	if (m_pBumpShader != NULL)
		m_pBumpShader->glStop();

	IRenderingProperties *props = IRenderingProperties::GetCurrentProperties();
	if (props->getCurrentTexturing() == IRenderingProperties::ENABLE)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);

#if defined(GL_ARB_texture_cube_map)
		if (m_pBumpShader->isEnabled())
		{
			pExtensions->glActiveTextureARB(GL_TEXTURE3_ARB);
			glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		}
#endif

		pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
	}
	
	CATCH_GL_ERROR
}

bool CBumppedGeometry::exportObject(CRaptorIO& o)
{
    return CGeometry::exportObject(o);
}

bool CBumppedGeometry::importObject(CRaptorIO& i)
{
    return CGeometry::importObject(i);
}
