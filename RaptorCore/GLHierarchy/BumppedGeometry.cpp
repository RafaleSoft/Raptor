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
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "Shader.h"
#endif
#if !defined(AFX_TEXTUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "TextureUnitSetup.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#ifndef __INTERNAL_PROCS_H__
	#include "Subsys/InternalProcs.h"
#endif
#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "Light.h"
#endif
#if !defined(AFX_VERTEXSHADER_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)
	#include "VertexShader.h"
#endif
#if !defined(AFX_FRAGMENTSHADER_H__66B3089A_2919_4678_9273_6CDEF7E5787F__INCLUDED_)
	#include "FragmentShader.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_BUMPLIGHTOBSERVER_H__238FC166_A3BC_4D77_8FD4_0A42DB45280F__INCLUDED_)
    #include "Subsys/BumpLightObserver.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "ObjectFactory.h"
#endif
#if !defined(AFX_3DENGINEMATRIX_H__6CD1110E_1174_4f38_A452_30FB312022D0__INCLUDED_)
	#include "Engine/3DEngineMatrix.h"
#endif
#if !defined(AFX_REFERENCE_H__D29BE5EA_DA55_4BCA_A700_73E007EFE5F9__INCLUDED_)
	#include "GLHierarchy/Reference.cxx"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Static data
CBumpLightObserver *CBumppedGeometry::m_pObserver = NULL;

static CBumppedGeometry::CBumppedGeometryClassID bumpId;
static CPersistentObjectType<CBumppedGeometry> bumpFactory(bumpId);
const CPersistence::CPersistenceClassID& CBumppedGeometry::CBumppedGeometryClassID::GetClassId(void)
{
	return bumpId;
}

CShader* CBumppedGeometry::m_pBumpShader2Lights = NULL;
CShader* CBumppedGeometry::m_pBumpShaderAmbient = NULL;
CShader* CBumppedGeometry::m_pBumpShader3Lights = NULL;
//////////////////////////////////////////////////////////////////////


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
	CGeometry(name,classID)
{
	init();
}

void CBumppedGeometry::init(void)
{
    if (m_pObserver == NULL)
        m_pObserver = new CBumpLightObserver();
    else
        m_pObserver->addReference();

#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((!Raptor::glIsExtensionSupported("GL_ARB_fragment_program")) ||
		(!Raptor::glIsExtensionSupported("GL_ARB_vertex_program")) ||
		(!Raptor::glIsExtensionSupported("GL_ARB_fragment_shader")) ||
		(!Raptor::glIsExtensionSupported("GL_ARB_vertex_shader")))
	{
		Raptor::GetErrorManager()->generateRaptorError(	CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_NO_GPU_PROGRAM);
	}
#endif

	m_pBumpShader = new CShader();
	m_pBumpShader->registerDestruction(this);

	if (m_pBumpShaderAmbient == NULL)
		m_pBumpShaderAmbient = new CShader("BUMP_SHADER_AMBIENT");
	else
		m_pBumpShaderAmbient->addReference();
	m_pBumpShaderAmbient->registerDestruction(this);

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
}


CBumppedGeometry::~CBumppedGeometry()
{
    diffuseMap = NULL;
	normalMap = NULL;
	envMap = NULL;

    if (NULL != m_pObserver) 
	{
		bool lastObject = (m_pObserver->getRefCount() == 1);
		m_pObserver->releaseReference();
		if (lastObject)
			m_pObserver = NULL;
	}

	if (m_pBumpShader != NULL)
	{
		m_pBumpShader->unregisterDestruction(this);
		m_pBumpShader->releaseReference();
	}

	if (m_pBumpShaderAmbient != NULL)
	{
		m_pBumpShaderAmbient->unregisterDestruction(this);
		m_pBumpShaderAmbient->releaseReference();
	}
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
}

void CBumppedGeometry::unLink(const CPersistence* p)
{
    if (p == static_cast<CPersistence*>(m_pBumpShader))
        m_pBumpShader = NULL;
    else if (p == static_cast<CPersistence*>(m_pBumpShaderAmbient))
        m_pBumpShaderAmbient = NULL;
	else if (p == static_cast<CPersistence*>(m_pBumpShader2Lights))
        m_pBumpShader2Lights = NULL;
	else if (p == static_cast<CPersistence*>(m_pBumpShader3Lights))
        m_pBumpShader3Lights = NULL;
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

	CVertexShader *vs = m_pBumpShader->glGetVertexShader("BUMP_ATT_VTX_SHADER");
	vs = m_pBumpShader2Lights->glGetVertexShader("BUMP_ATT_VTX_SHADER_2LIGHTS");
	vs = m_pBumpShaderAmbient->glGetVertexShader("BUMP_0LIGHT_VTX_SHADER");
	vs = m_pBumpShader3Lights->glGetVertexShader("BUMP_ATT_VTX_SHADER_3LIGHTS");

	 if ((normalMap == NULL) || (diffuseMap == NULL))
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

	 if ((!Raptor::glIsExtensionSupported("GL_NV_register_combiners")) &&
		 (!Raptor::glIsExtensionSupported("GL_ARB_fragment_program")))
	 {
		 Raptor::GetErrorManager()->generateRaptorError(CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_NO_GPU_PROGRAM);
		 return;
	 }
	 if (!Raptor::glIsExtensionSupported("GL_EXT_secondary_color"))
	 {
		 Raptor::GetErrorManager()->generateRaptorError(CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_NO_GPU_PROGRAM);
		 return;
	 }

#if defined (GL_ARB_fragment_program)
	 setup->useRegisterCombiners(false);
	 CFragmentShader *fs = m_pBumpShader->glGetFragmentShader("BUMP_ATT_TEX_SHADER");
	 fs = m_pBumpShader2Lights->glGetFragmentShader("BUMP_ATT_TEX_SHADER_2LIGHTS");
	 fs = m_pBumpShaderAmbient->glGetFragmentShader("BUMP_0LIGHT_TEX_SHADER");
	 fs = m_pBumpShader3Lights->glGetFragmentShader("BUMP_ATT_TEX_SHADER_3LIGHTS");
#endif

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

unsigned int CBumppedGeometry::glUpdateLightPosition(void)
{
	unsigned int numLights = 0;
 	C3DEngineMatrix T;

	glGetTransposeFloatv(GL_MODELVIEW_MATRIX,T);

    GL_COORD_VERTEX center;
    getCenter(center);
	CGenericVector<float> x(center.x,center.y,center.z,1.0f);
    x *= T;
	vector<CLight*> lights = m_pObserver->sortLights(x);
	if (lights.size() < 1)
		return numLights;
	CLight *pMainLight = lights[0];

	V.x = -(T[0]*T[3] + T[4]*T[7] + T[8]*T[11]);
	V.y = -(T[1]*T[3] + T[5]*T[7] + T[9]*T[11]);
	V.z = -(T[2]*T[3] + T[6]*T[7] + T[10]*T[11]);

	T.Inverse();

	numLights++;
    X = T * pMainLight->getLightEyePosition();

	
	GL_COORD_VERTEX att = pMainLight->getSpotParams();
    A.x = att.z;
    A.y = att.y;
    A.z = att.x;
    A.h = 0.0f;
	S = pMainLight->getSpecular();

	if (lights.size() > 1)
	{
		CLight *pSecondLight = lights[1];
		numLights++;
		X2 = T * pSecondLight->getLightEyePosition();

		att = pSecondLight->getSpotParams();
		A2.x = att.z;
		A2.y = att.y;
		A2.z = att.x;
		A2.h = 0.0f;
		S2 = pSecondLight->getSpecular();
	}

	if (lights.size() > 2)
	{
		CLight *pThirdLight = lights[2];
		numLights++;
		X3 = T * pThirdLight->getLightEyePosition();

		att = pThirdLight->getSpotParams();
		A3.x = att.z;
		A3.y = att.y;
		A3.z = att.x;
		A3.h = 0.0f;
		S3 = pThirdLight->getSpecular();
	}

	return numLights;
}


void CBumppedGeometry::glRender()
{
	if (!properties.isVisible())
		return;

#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if (diffuseMap == NULL)
	{
			return;
			Raptor::GetErrorManager()->generateRaptorError(	CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
															CRaptorMessages::ID_TEXTURE_MISS);
	}
#endif

	unsigned int numLights = glUpdateLightPosition();
		
    m_pBumpShader->glRenderMaterial();
    // for debug  ambient : glColor4f(1.0f,0.0f,0.0f,1.0f);

	if ((!glIsEnabled(GL_LIGHTING)) || (numLights == 0))
    {
		m_pBumpShaderAmbient->glRender();

		m_pBumpShader->glRenderTexture();
		CGeometry::glRender();

		m_pBumpShaderAmbient->glStop();
    }
	else
	{
		CFragmentShader *fs = NULL;
		CVertexShader *vs = NULL;

		if (numLights == 1)
		{
			fs = m_pBumpShader->glGetFragmentShader();
			fs->glRender();
			fs->glProgramParameter(0,S);
			vs = m_pBumpShader->glGetVertexShader();
			vs->glRender();
			vs->glProgramParameter(4,X);
			vs->glProgramParameter(5,V);
			vs->glProgramParameter(6,A);
		}
		else if (numLights == 2)
		{
			fs = m_pBumpShader2Lights->glGetFragmentShader();
			fs->glRender();
			fs->glProgramParameter(0,S);
			fs->glProgramParameter(1,S2);

			vs = m_pBumpShader2Lights->glGetVertexShader();
			vs->glRender();
			vs->glProgramParameter(4,X);
			vs->glProgramParameter(5,V);
			vs->glProgramParameter(6,A);
			vs->glProgramParameter(7,X2);
			vs->glProgramParameter(8,A2);
		}
		else
		{
			fs = m_pBumpShader3Lights->glGetFragmentShader();
			fs->glRender();
			fs->glProgramParameter(0,S);
			fs->glProgramParameter(1,S2);
			fs->glProgramParameter(2,S3);

			vs = m_pBumpShader3Lights->glGetVertexShader();
			vs->glRender();
			vs->glProgramParameter(4,X);
			vs->glProgramParameter(5,V);
			vs->glProgramParameter(6,A);
			vs->glProgramParameter(7,X2);
			vs->glProgramParameter(8,A2);
			vs->glProgramParameter(9,X3);
			vs->glProgramParameter(10,A3);
		}

        m_pBumpShader->glRenderTexture();

        glRenderGeometry();

		vs->glStop();
		fs->glStop();

#if defined(GL_ARB_multitexture)
        const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
	    if (pExtensions->glClientActiveTextureARB != NULL)
	    {
		    pExtensions->glActiveTextureARB(GL_TEXTURE1_ARB);
		    glDisable(GL_TEXTURE_2D);
		    pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
        }
#endif
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

