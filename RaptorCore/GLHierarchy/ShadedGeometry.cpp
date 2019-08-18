// ShadedGeometry.cpp: implementation of the CShadedGeometry class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_SHADEDGEOMETRY_H__E56C66F7_2DF6_497B_AA0F_19DDC11390F9__INCLUDED_)
    #include "ShadedGeometry.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_AMBIENTOCCLUSIONSHADER_H__FA8234C4_82B1_49D3_ABAA_7FCE45EDDCAD__INCLUDED_)
	#include "Subsys/AmbientOcclusionShader.h"
#endif
#if !defined(AFX_VERTEXPROGRAM_OLD_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)
	#include "VertexProgram_old.h"
#endif
#if !defined(AFX_FRAGMENTSHADER_H__66B3089A_2919_4678_9273_6CDEF7E5787F__INCLUDED_)
	#include "FragmentShader.h"
#endif
#if !defined(AFX_VULKANSHADERSTAGE_H__EF5769B8_470D_467F_9FDE_553142C81698__INCLUDED_)
	#include "VulkanShaderStage.h"
#endif
#if !defined(AFX_RAPTORVULKANSHADER_H__C188550F_1D1C_4531_B0A0_727CE9FF9450__INCLUDED_)
	#include "Subsys/Vulkan/VulkanShader.h"
#endif
#if !defined(AFX_OPENGLRENDERINGPROPERTIES_H__1F0F1E67_FC84_4772_A6EE_923BD81F91D3__INCLUDED_)
	#include "Subsys/OpenGL/OpenGLRenderingProperties.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "ObjectFactory.h"
#endif


RAPTOR_NAMESPACE

static CShadedGeometry::CShadedGeometryClassID shadedId;
static CPersistentObjectType<CShadedGeometry> geometryFactory(shadedId);
const CPersistence::CPersistenceClassID& CShadedGeometry::CShadedGeometryClassID::GetClassId(void)
{
	return shadedId;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CShadedGeometry::CShadedGeometry(const std::string& name)
	:CGeometry(name,shadedId),
	m_pShader(NULL),m_pAOShader(NULL),m_pOverride(NULL)
{
}

CShadedGeometry::~CShadedGeometry()
{
	if (m_pShader !=NULL)
    {
        m_pShader->unregisterDestruction(this);
		m_pShader->releaseReference();
    }
	if (m_pAOShader != NULL)
    {
        m_pAOShader->unregisterDestruction(this);
		m_pAOShader->releaseReference();
    }
    if (m_pOverride != NULL)
        delete m_pOverride;
}

CShader * const CShadedGeometry::getShader(void)
{
	if (m_pShader == NULL)
	{
		m_pShader = new CShader(getName()+"_Shader");
		m_pShader->registerDestruction(this);
	}
	return m_pShader;
}

CShader * const CShadedGeometry::getAmbientOcclusionShader(void)
{
	if (m_pAOShader == NULL)
	{
		int width = ceil(log(sqrt((float)nbVertex())) / log(2.0));
		int height = width = pow(2.0,width);

		float px_size = 2.0f / width;
		float py_size = 2.0f / height;
		float x_base = -1.0f + 0.5f*px_size;
		float y_base = -1.0f + 0.5f*py_size;

		GL_TEX_VERTEX *coords = new GL_TEX_VERTEX[nbVertex()];
		for (unsigned int i=0;i<nbVertex();i++)
		{
			int x = i % width;
			int y = i / width;
			coords[i].u = x_base + x*px_size;
			coords[i].v = y_base + y*py_size;
		}
		
		glSetTexCoords2(nbVertex(),NULL);
		glSetTexCoords2(nbVertex(),coords);
		delete [] coords;

		CAmbientOcclusionShader *AO = NULL;

		glLockData();

		{
			AO = new CAmbientOcclusionShader(getName()+"_AOShader");
			AO->glSetCoords(vertex,nbVertex());
			AO->glSetNormals(normals,nbVertex());
			AO->registerDestruction(this);
			m_pAOShader = AO;
		}

		glUnLockData();

		// This call will reassign pointers to data after relocation
		// (due to glUnlockData)
		AO->glSetRefData(vertex,normals,texcoords2);
	}
	return m_pAOShader;
}

void CShadedGeometry::setShader(CShader *shader)
{
    if (shader == m_pShader)
        return ;

    if (m_pShader != NULL)
    {
		m_pShader->unregisterDestruction(this);
        m_pShader->releaseReference();
        m_pShader = NULL;
    }

    if (shader != NULL)
    {
        m_pShader = shader;
        m_pShader->addReference();
        m_pShader->registerDestruction(this);
    }
}

void CShadedGeometry::overrideShading(const IRenderingProperties& override)
{
    if (m_pOverride ==  NULL)
		m_pOverride = new COpenGLRenderingProperties(override);
    else
        *m_pOverride = override;

    //  prevent buffer clear
    m_pOverride ->clear(0);
}

void CShadedGeometry::vkRender(	CVulkanCommandBuffer& commandBuffer,
								VkBuffer vertexBinding,
								VkBuffer indexBinding)
{
	if (m_pShader != NULL)
	{
		if (m_pShader->hasVulkanProgram())
		{
			CTextureUnitSetup *tmus = NULL;
			if (m_pShader->hasTextureUnitSetup())
				tmus = m_pShader->glGetTextureUnitsSetup();
			CVulkanShaderStage *ss = m_pShader->vkGetVulkanProgram();
			ss->vkRender(commandBuffer, tmus);
		}
	}

	CGeometry::vkRender(commandBuffer, vertexBinding, indexBinding);
}

void CShadedGeometry::glRender()
{
	if (!properties.isVisible())
		return;

	if (m_pOverride != NULL)
		m_pOverride->glPushProperties();
	
	if (m_pShader != NULL)
	{
		// apply material
		if (getRenderingModel().hasModel(CRenderingModel::CGL_NORMALS))
		{
		    m_pShader->glRenderMaterial();
		}

		// apply texture
		if (getRenderingModel().hasModel(CRenderingModel::CGL_TEXTURE))
		{
		    m_pShader->glRenderTexture();
			if (m_pAOShader != NULL)
				((CAmbientOcclusionShader *)m_pAOShader)->glRenderResult();
		}

		// render shaders
		m_pShader->glRender();
	}
	
	CATCH_GL_ERROR

	glRenderGeometry();

	if (m_pShader != NULL)
		m_pShader->glStop();

    if (m_pOverride != NULL)
        m_pOverride->glPopProperties();
}

CShadedGeometry& CShadedGeometry::operator=(const CGeometry& geometry)
{
	CGeometry::operator=(geometry);

	return *this;
}

CShadedGeometry& CShadedGeometry::operator=(const CShadedGeometry& geometry)
{
	CGeometry::operator=(geometry);

    setShader(geometry.m_pShader);

	return *this;
}

void CShadedGeometry::unLink(const CPersistence* obj)
{
    if (obj == static_cast<CPersistence*>(m_pShader))
        m_pShader = NULL;
	else if (obj == static_cast<CPersistence*>(m_pAOShader))
        m_pAOShader = NULL;
    else
        CGeometry::unLink(obj);
}

bool CShadedGeometry::exportObject(CRaptorIO& o)
{
    return true;
}

bool CShadedGeometry::importObject(CRaptorIO& io)
{
    string name;
    io >> name;

	string data = io.getValueName();
    while (!data.empty())
    {
		if (data == "name")
			CPersistence::importObject(io);
		else if (data == "Geometry")
			CGeometry::importObject(io);
		else if (data == "RenderingProperties")
		{
			COpenGLRenderingProperties props;
            props.importObject(io);
            overrideShading(props);
		}
		else if (data == "Shader")
		{
			io >> name;
			data = io.getValueName();
			if (data == "name")
			{
				io >> name;
				CPersistence* object = CPersistence::FindObject(name);
				if ((object != NULL) && 
					(object->getId().isSubClassOf(CShader::CShaderClassID::GetClassId())))
					setShader((CShader*)object);
			}
			io >> name;
		}
		else
			io >> name;
		
		data = io.getValueName();
	}
	io >> name;

	return true;
}

