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
#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/3DEngine.h"
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
#ifndef __GLOBAL_H__
	#include "System/Global.h"
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
	//	Init dynamic light arrays
	bumpDiffusePx = NULL;
	bumpSpecularPx = NULL;

    if (m_pObserver == NULL)
        m_pObserver = new CBumpLightObserver();
    else
        m_pObserver->addReference();

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

	if (NULL != bumpDiffusePx)	delete [] bumpDiffusePx;
	if (NULL != bumpSpecularPx)	delete [] bumpSpecularPx;

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

void CBumppedGeometry::setDiffuseMap(CTextureObject* diffuse) 
{ 
	diffuseMap = diffuse;
}

void CBumppedGeometry::setNormalMap(CTextureObject* normal) 
{ 
    normalMap = normal;
}

void CBumppedGeometry::setEnvironmentMap(CTextureObject* environment) 
{ 
    envMap = environment;
}

void CBumppedGeometry::setRenderingModel(const CRenderingModel& model)
{
	if (model.hasModel(CRenderingModel::CGL_DOT3_BUMP))
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		if (!Raptor::glIsExtensionSupported("GL_ARB_texture_env_dot3"))
		{
            Raptor::GetErrorManager()->generateRaptorError(	CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
														 	CRaptorErrorManager::RAPTOR_WARNING,
															CRaptorMessages::ID_NO_GPU_DOT3);
		}
#endif
	}

	if (model.hasModel(CRenderingModel::CGL_SHADER_BUMP) ||
        model.hasModel(CRenderingModel::CGL_SHADER_EMBM))
	{
		const CGeometryEditor &pEditor = getEditor();
        if (normals == NULL)
            pEditor.genNormals(true);
        else if (tangents == NULL)
            pEditor.genNormals(false);
		pEditor.genBinormals();

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
	}

    //  render material normals is mandatory for this kind of geometry
    CRenderingModel l_model = model;
	l_model.addModel(CRenderingModel::CGL_NORMALS);
    l_model.addModel(CRenderingModel::CGL_TANGENTS);
    CGeometry::setRenderingModel(l_model);

	glBuildShader();

	CATCH_GL_ERROR
}


void CBumppedGeometry::glBuildShader(void)
{
	if (getRenderingModel().hasModel(CRenderingModel::CGL_SHADER_BUMP))
	{
        CVertexShader *vs = m_pBumpShader->glGetVertexShader("BUMP_ATT_VTX_SHADER");
		vs = m_pBumpShader2Lights->glGetVertexShader("BUMP_ATT_VTX_SHADER_2LIGHTS");
        vs = m_pBumpShaderAmbient->glGetVertexShader("BUMP_0LIGHT_VTX_SHADER");
		vs = m_pBumpShader3Lights->glGetVertexShader("BUMP_ATT_VTX_SHADER_3LIGHTS");
	}
	else if (getRenderingModel().hasModel(CRenderingModel::CGL_SHADER_EMBM))
	{
		CVertexShader *vs = m_pBumpShader->glGetVertexShader("EMBM_VTX_SHADER");
        vs = m_pBumpShaderAmbient->glGetVertexShader("EMBM_0LIGHT_VTX_SHADER");
	}
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	else if (!getRenderingModel().hasModel(CRenderingModel::CGL_BUMP_MAPPING))
        Raptor::GetErrorManager()->generateRaptorError(	CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_WRONG_RENDERING);
#endif

	glSetTextureUnits();

	CATCH_GL_ERROR
}


void CBumppedGeometry::glSetTextureUnits(void)
{
	if ((normalMap == NULL) || (diffuseMap == NULL))
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(	CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
										                CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_TEXTURE_MISS);
#endif
		return;
	}

	if (getRenderingModel().hasModel(CRenderingModel::CGL_DOT3_BUMP))
	{
        //  Dot3 rendering needs normalMap on TMU0 !
		CTextureUnitSetup *setup = m_pBumpShader->glGetTextureUnitsSetup();
		setup->setDiffuseMap(normalMap);
		setup->setNormalMap(diffuseMap);
		
		//	This case is degenerate. The builder prevents
		//	falling into this inconsistency
#if defined(GL_ARB_texture_env_dot3)
        setup->getTMUCombiner(CTextureUnitSetup::IMAGE_UNIT_0).rgb_combiner = true;
		setup->getTMUCombiner(CTextureUnitSetup::IMAGE_UNIT_0).rgb_op = GL_DOT3_RGB_ARB;
		setup->getTMUCombiner(CTextureUnitSetup::IMAGE_UNIT_0).src_rgb_0 = GL_TEXTURE;
		setup->getTMUCombiner(CTextureUnitSetup::IMAGE_UNIT_0).src_rgb_1 = GL_PRIMARY_COLOR_ARB;

		setup->getTMUCombiner(CTextureUnitSetup::IMAGE_UNIT_1).rgb_combiner = true;
		setup->getTMUCombiner(CTextureUnitSetup::IMAGE_UNIT_1).rgb_op = GL_MODULATE;
		setup->getTMUCombiner(CTextureUnitSetup::IMAGE_UNIT_1).src_rgb_0 = GL_TEXTURE;
		setup->getTMUCombiner(CTextureUnitSetup::IMAGE_UNIT_1).src_rgb_1 = GL_PREVIOUS_ARB;
#endif
		setup->useRegisterCombiners(false);
	}
    else if (getRenderingModel().hasModel(CRenderingModel::CGL_SHADER_EMBM))
    {
        if (envMap == NULL)
	    {
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	        Raptor::GetErrorManager()->generateRaptorError(	CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
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
    }
	else if (getRenderingModel().hasModel(CRenderingModel::CGL_SHADER_BUMP))
	{
		CTextureUnitSetup *setup = m_pBumpShader->glGetTextureUnitsSetup();
		setup->setDiffuseMap(diffuseMap);
		setup->setNormalMap(normalMap);

		if ((!Raptor::glIsExtensionSupported("GL_NV_register_combiners")) &&
			(!Raptor::glIsExtensionSupported("GL_ARB_fragment_program")))
		{
			Raptor::GetErrorManager()->generateRaptorError(	CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
                                                            CRaptorErrorManager::RAPTOR_WARNING,
                                                            CRaptorMessages::ID_NO_GPU_PROGRAM);
			return;
		}
		if (!Raptor::glIsExtensionSupported("GL_EXT_secondary_color"))
		{
			Raptor::GetErrorManager()->generateRaptorError(	CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
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
	}

	CATCH_GL_ERROR
}


void CBumppedGeometry::allocBumpColors()
{
	if (!m_pBumpShader->hasVertexShader())
	{
		if (bumpDiffusePx == NULL)
			bumpDiffusePx = new GLubyte[4*m_nbVertex];
		if (bumpSpecularPx == NULL)
			bumpSpecularPx = new GLubyte[4*m_nbVertex];
	}
}

CBumppedGeometry& CBumppedGeometry::operator=(const CGeometry &geo)
{
	CGeometry::operator =(geo);

	if (bumpDiffusePx != NULL) delete [] bumpDiffusePx;
	if (bumpSpecularPx != NULL) delete [] bumpSpecularPx;

    const CGeometryEditor &pEditor = getEditor();
	pEditor.genBinormals();

	return *this;
}

CBumppedGeometry& CBumppedGeometry::operator=(const CBumppedGeometry &geo)
{
    CGeometry::operator =(geo);

	if (bumpDiffusePx != NULL) delete [] bumpDiffusePx;
	if (bumpSpecularPx != NULL) delete [] bumpSpecularPx;

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
    CLight *pMainLight = m_pObserver->getLight(x,0);
	if (pMainLight == NULL)
		return numLights;

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

	CLight *pSecondLight = m_pObserver->getLight(x,1);
	if(pSecondLight != NULL)
	{
		numLights++;
		X2 = T * pSecondLight->getLightEyePosition();

		att = pSecondLight->getSpotParams();
		A2.x = att.z;
		A2.y = att.y;
		A2.z = att.x;
		A2.h = 0.0f;
		S2 = pSecondLight->getSpecular();
	}

	CLight *pThirdLight = m_pObserver->getLight(x,2);
	if(pThirdLight != NULL)
	{
		numLights++;
		X3 = T * pThirdLight->getLightEyePosition();

		att = pThirdLight->getSpotParams();
		A3.x = att.z;
		A3.y = att.y;
		A3.z = att.x;
		A3.h = 0.0f;
		S3 = pThirdLight->getSpecular();
	}

	if (!m_pBumpShader->hasVertexShader())
	{
		if (bumpDiffusePx == NULL)
			allocBumpColors();

#ifdef RAPTOR_SSE_CODE_GENERATION
		if (Global::GetInstance().getCurrentStatus().forceSSE)
			setLightPositionDOT3SSE();
		else
			setLightPositionDOT3();
#else
		setLightPositionDOT3();
#endif

	}

	return numLights;
}


void CBumppedGeometry::setLightPositionDOT3(void)
{
	//	inverse transform
	CGenericVector<float>	diffuse;
	GL_COORD_VERTEX	specular;
	CGenericVector<float>	viewer;

	float m_fShininess = normalMap->getTransparency();
	float d,d2;
	int index = 0;

	for (unsigned int i=0;i<m_nbVertex;i++)
	{
		GL_COORD_VERTEX L;

		diffuse.X() = X.X() - VERTEX(i).x;
		diffuse.Y() = X.Y() - VERTEX(i).y;
		diffuse.Z() = X.Z() - VERTEX(i).z;
		diffuse.Normalize();

		d = diffuse.X() * NORMAL(i).x + diffuse.Y() * NORMAL(i).y + diffuse.Z() * NORMAL(i).z;
		
		// 1st test : no light
		if (d < 0)
		{
			*((unsigned int*)&bumpDiffusePx[index]) = 0xFF808080;
			*((unsigned int*)&bumpSpecularPx[index]) = 0xFF808080;
		}
		else
		{
			viewer.X() = V.x - VERTEX(i).x;
			viewer.Y() = V.y - VERTEX(i).y;
			viewer.Z() = V.z - VERTEX(i).z;
			viewer.Normalize();

			d2 = viewer.X() * NORMAL(i).x + viewer.Y() * NORMAL(i).y + viewer.Z() * NORMAL(i).z;

			// 2nd test : no view
			if (d2 < 0)
			{
				*((unsigned int*)&bumpDiffusePx[index]) = 0xFF808080;
				*((unsigned int*)&bumpSpecularPx[index]) = 0xFF808080;
			}
			else
			{
				L.z = 127.0f * d;
				L.x = 127.0f * (diffuse.X() * TANGENT(i).x + diffuse.Y() * TANGENT(i).y + diffuse.Z() * TANGENT(i).z);
				L.y = 127.0f * (diffuse.X() * BINORMAL(i).x + diffuse.Y() * BINORMAL(i).y + diffuse.Z() * BINORMAL(i).z);
			
				bumpDiffusePx[index] = ((unsigned char)(L.x + 128));
				bumpDiffusePx[index+1] = ((unsigned char)(L.y + 128));
				bumpDiffusePx[index+2] = ((unsigned char)(L.z + 128));
			
				specular.x = 0.5f * (viewer.X() + diffuse.X());
				specular.y = 0.5f * (viewer.Y() + diffuse.Y());
				specular.z = 0.5f * (viewer.Z() + diffuse.Z());

				L.z = 0.5f * m_fShininess * (specular.x * NORMAL(i).x + specular.y * NORMAL(i).y + specular.z * NORMAL(i).z);
				L.x = 0.5f * m_fShininess * (specular.x * TANGENT(i).x + specular.y * TANGENT(i).y + specular.z * TANGENT(i).z);
				L.y = 0.5f * m_fShininess * (specular.x * BINORMAL(i).x + specular.y * BINORMAL(i).y + specular.z * BINORMAL(i).z);
				
				bumpSpecularPx[index] = ((unsigned char)(L.x + 128));
				bumpSpecularPx[index+1] = ((unsigned char)(L.y + 128));
				bumpSpecularPx[index+2] = ((unsigned char)(L.z + 128));
			}
		}

		index+=4;
	}
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

    if (getRenderingModel().hasModel(CRenderingModel::CGL_SHADER_EMBM))
        glRenderEMBM();
    else if ((!glIsEnabled(GL_LIGHTING)) || (numLights == 0))
    {
		m_pBumpShaderAmbient->glRender();

		m_pBumpShader->glRenderTexture();
		CGeometry::glRender();

		m_pBumpShaderAmbient->glStop();
    }
	else if (getRenderingModel().hasModel(CRenderingModel::CGL_DOT3_BUMP))
		glRenderDOT3();
	else if (getRenderingModel().hasModel(CRenderingModel::CGL_SHADER_BUMP))
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
			fs->glProgramParameter(1,S3);

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

	Global::GetInstance().getCurrentStatus().iRenderedObjects++;
	Global::GetInstance().getCurrentStatus().iRenderedTriangles += m_nbPolys;

	CATCH_GL_ERROR
}


void CBumppedGeometry::glRenderDOT3()
{
	GLint func;
	glGetIntegerv(GL_DEPTH_FUNC,&func);
	GLboolean light;
  	glGetBooleanv(GL_LIGHTING,&light);
	glDisable(GL_LIGHTING);  	

	//	1st pass to render diffuse and z-buffer
	glBlendFunc(GL_ONE,GL_ZERO);

	glEnableClientState(GL_VERTEX_ARRAY);
#if defined(DATA_EXTENDED)
    glVertexPointer( 3,GL_FLOAT,sizeof(GL_VERTEX_DATA),&geometry[0].vertex);
#elif defined(DATA_PACKED)
	glVertexPointer( 3,GL_FLOAT,sizeof(GL_COORD_VERTEX),vertex);
#endif

	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
#if defined(GL_EXT_compiled_vertex_array)
	if (pExtensions->glLockArraysEXT != NULL)
		pExtensions->glLockArraysEXT(0,m_nbVertex);
#endif

	//	diffuse
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4,GL_UNSIGNED_BYTE,0,bumpDiffusePx);
	
	m_pBumpShader->glRenderTexture();

#if defined(GL_ARB_multitexture)
	if (pExtensions->glClientActiveTextureARB != NULL)
	{
		pExtensions->glClientActiveTextureARB(GL_TEXTURE0_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#if defined(DATA_EXTENDED)
        glTexCoordPointer( 2,GL_FLOAT,sizeof(GL_VERTEX_DATA),&geometry[0].texCoord0);
#elif defined(DATA_PACKED)
		glTexCoordPointer( 2,GL_FLOAT,0,texcoords);
#endif
		pExtensions->glClientActiveTextureARB(GL_TEXTURE1_ARB);
	}
#endif
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#if defined(DATA_EXTENDED)
        glTexCoordPointer( 2,GL_FLOAT,sizeof(GL_VERTEX_DATA),&geometry[0].texCoord0);
#elif defined(DATA_PACKED)
	glTexCoordPointer( 2,GL_FLOAT,0,texcoords);
#endif

	glDrawElements( GL_TRIANGLES, 3*m_nbPolys, GL_UNSIGNED_SHORT,polys);

#if defined(GL_ARB_multitexture)
	if (pExtensions->glClientActiveTextureARB != NULL)
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		pExtensions->glClientActiveTextureARB(GL_TEXTURE0_ARB);
		pExtensions->glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
	}
#endif

	//	2nd pass to render specular
	glBlendFunc(GL_ONE,GL_ONE);
	glDepthFunc(GL_EQUAL);

	//	specular
	
	glColorPointer(4,GL_UNSIGNED_BYTE,0,bumpSpecularPx);
	normalMap->glRender();

#if defined(GL_ARB_texture_env_dot3)
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE_ARB);
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_ARB,GL_DOT3_RGB_ARB);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB_ARB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB_ARB,GL_PRIMARY_COLOR_ARB);
#endif
	glDrawElements( GL_TRIANGLES, 3*m_nbPolys, GL_UNSIGNED_SHORT,polys);
	
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

#if defined(GL_EXT_compiled_vertex_array)
	if (pExtensions->glUnlockArraysEXT != NULL)
		pExtensions->glUnlockArraysEXT();
#endif

	glDepthFunc(func);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	if (light)
		glEnable(GL_LIGHTING);

	CATCH_GL_ERROR
}

void CBumppedGeometry::glRenderEMBM()
{
    bool renderAmbient = (GL_TRUE == glIsEnabled(GL_LIGHTING));

    if (getRenderingModel().hasModel(CRenderingModel::CGL_TEXTURE))
    {
        m_pBumpShader->glRenderTexture();
    }

    CVertexShader *vs = NULL;
    CFragmentShader *fs = NULL;

    if (!renderAmbient)
    {
        vs = m_pBumpShaderAmbient->glGetVertexShader();
        vs->glRender();
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer( GL_FLOAT , sizeof(GL_COORD_VERTEX) , normals);
        fs = m_pBumpShaderAmbient->glGetFragmentShader();
    }
    else
    {
        vs = m_pBumpShader->glGetVertexShader();
		vs->glRender();
		vs->glProgramParameter(10,X);
		vs->glProgramParameter(11,V);
        vs->glProgramParameter(12,A);
        fs = m_pBumpShader->glGetFragmentShader();
    }
	
    fs->glRender();
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
    if (!renderAmbient)
        glDisableClientState(GL_NORMAL_ARRAY);

    vs->glStop();
    fs->glStop();
    
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

