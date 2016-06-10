// AmbientOcclusionShader.cpp: implementation of the CAmbientOcclusionShader class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_AMBIENTOCCLUSIONSHADER_H__FA8234C4_82B1_49D3_ABAA_7FCE45EDDCAD__INCLUDED_)
	#include "AmbientOcclusionShader.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
	#include "GLHierarchy/TextureFactoryConfig.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORDISPLAYCONFIG_H__DA0759DF_6CF9_44A7_9ADE_D404FEEC2DDF__INCLUDED_)
	#include "System/RaptorDisplayConfig.h"
#endif
#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "GLHierarchy/TextureSet.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
    #include "System/Raptor.h"
#endif
#if !defined(AFX_RENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/RenderingProperties.h"
#endif
#if !defined(AFX_TEXUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "GLHierarchy/TextureUnitSetup.h"
#endif
#if !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)
	#include "GLHierarchy/ShaderProgram.h"
#endif
#if !defined(AFX_VERTEXPROGRAM_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
    #include "GLHierarchy/VertexProgram.h"
#endif
#if !defined(AFX_FRAGMENTPROGRAM_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
    #include "GLHierarchy/FragmentProgram.h"
#endif
#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/3DEngine.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
#endif


RAPTOR_NAMESPACE


CAmbientOcclusionShader::CAmbientOcclusionShader(const std::string& name)
	:CShader(name),m_refVertex(NULL),m_refNormal(NULL),
	m_pVertexMap(NULL),m_pNormalMap(NULL),m_pAOMap(NULL),m_pAOBuffer(NULL),
	m_pAOcomputeRef(NULL)
{
}

CAmbientOcclusionShader::~CAmbientOcclusionShader(void)
{
}

void CAmbientOcclusionShader::setOccluders(const vector<CAmbientOcclusionShader*>& occluders,
										   const vector<CGenericMatrix<float>>& transforms)
{
	m_occluders = occluders;
	m_transforms = transforms;
}

bool CAmbientOcclusionShader::glInitAOCompute(void)
{
	if ((m_refVertex == NULL) || 
		(m_refNormal == NULL) ||
		(m_pVertexMap == NULL) ||
		(m_pNormalMap == NULL))
		return false;

	CTextureFactory &f = CTextureFactory::getDefaultFactory();
	f.getConfig().useTextureResize(false);
	CTextureObject *T = f.glCreateTexture(	CTextureObject::CGL_COLOR_FLOAT32_ALPHA,
											CTextureObject::CGL_OPAQUE,
											CTextureObject::CGL_BILINEAR);
	T->glUpdateClamping(CTextureObject::CGL_EDGECLAMP);
	f.glResizeTexture(T,m_pVertexMap->getWidth(),m_pVertexMap->getHeight());

	CTextureSet *pOutputTextures = new CTextureSet("Filter Output Render Textures");
	pOutputTextures->addTexture(T);
	T->releaseReference();
	m_pAOMap = T;

	CRaptorDisplayConfig cfg;
	cfg.width = m_pVertexMap->getWidth();
	cfg.height = m_pVertexMap->getHeight();
	cfg.acceleration = CRaptorDisplayConfig::HARDWARE;
	cfg.display_mode =	CGL_FLOAT |
						CGL_RENDER_TEXTURE |
						CGL_RENDER_BUFFER;

    m_pAOBuffer = Raptor::glCreateDisplay(cfg);
    CRenderingProperties *rp = m_pAOBuffer->getRenderingProperties();
    rp->setTexturing(CRenderingProperties::ENABLE);
    rp->setCullFace(CRenderingProperties::DISABLE);
    rp->setDepthTest(CRenderingProperties::DISABLE);
    rp->setLighting(CRenderingProperties::DISABLE);
    rp->clear(CGL_NULL);
    m_pAOBuffer->setViewPoint(NULL);
	m_pAOBuffer->glBindDisplay(*pOutputTextures);

	m_pAOcomputeRef = CShader::getShader("AOCOMPUTE_SHADER").glClone("AO_SHADER");
	CTextureUnitSetup *AOdata = glGetTextureUnitsSetup();
	AOdata->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_0,true);
	AOdata->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_1,true);
	AOdata->setDiffuseMap(m_pVertexMap);
	AOdata->setNormalMap(m_pNormalMap);
	m_AOMapSetup = AOdata->glBuildSetup();

	CATCH_GL_ERROR;

	if (!Raptor::glIsExtensionSupported("GL_ARB_texture_rectangle"))
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(	CShader::CShaderClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														"Raptor cannot render AmbientOcclusion: GPU is missing texture rectangle extension");
#endif
		return false;
	}

	return true;
}

void CAmbientOcclusionShader::glRenderResult()
{
	if (m_transforms.size() == 0)
		return;

	if (CRenderingProperties::GetCurrentProperties()->getCurrentTexturing() == CRenderingProperties::ENABLE)
	{
		const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glClientActiveTextureARB(GL_TEXTURE2_ARB);

		// This is responsibility of a geometry
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,GL_FLOAT,0,m_refTexCoords);

		//glEnable(GL_TEXTURE_2D);
		m_pAOMap->glRender();
/*
		{
			CColor::RGBA *img = new CColor::RGBA[64*64];
			glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_FLOAT,img);
			for (unsigned int p=0;p<m_refNbVertex;p++)
			{
				CColor::RGBA& c = img[p];
				// Compare img with GeometryEditer::updateSurfaceElements result
				// (result computed in colors)
				//CColor::RGBA& c2 = colors[p];
				if (c.r < 1.0f)
				{
					c.r = 0.0f;
				}
			}
			delete [] img;
		}
*/
		pExtensions->glClientActiveTextureARB(GL_TEXTURE0_ARB);
	}
}

void CAmbientOcclusionShader::glRender()
{
	if ((m_refVertex == NULL) || 
		(m_refNormal == NULL) ||
		(m_pVertexMap == NULL) ||
		(m_pNormalMap == NULL))
		return;

	if (m_transforms.size() == 0)
		return;

	// Prepare shader parameters: 
	//	- transforms
	CGenericMatrix<float> T1;
	T1.Ident();
	CGenericMatrix<float> T2_inv(m_transforms[0]);
	T2_inv.Inv();

	CGenericMatrix<float> M = T2_inv * T1;
	CGenericMatrix<float> M_inv(M);
	M_inv.Inv().Transpose();
	GL_MATRIX vertexMat;
	GL_MATRIX normalMat;
	C3DEngine::Generic_to_MATRIX(vertexMat,M);
	C3DEngine::Generic_to_MATRIX(normalMat,M_inv);

	CShaderProgram::CProgramParameters v_params;
	v_params.addParameter("vertexMat",vertexMat);
	v_params.addParameter("normalMat",normalMat);

	// Prepare shader parameters: 
	//	- texture maps
	CShaderProgram::CProgramParameters f_params;
	f_params.addParameter("posMap",CTextureUnitSetup::IMAGE_UNIT_0);
	f_params.addParameter("normalMap",CTextureUnitSetup::IMAGE_UNIT_1);
	f_params.addParameter("numRows",GL_COORD_VERTEX(m_occluders[0]->m_refNbVertex/64,0,0,0));

	RAPTOR_HANDLE noDevice;
	m_pAOBuffer->glBindDisplay(noDevice);

#ifdef GL_ARB_texture_rectangle
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
#endif
	m_pAOcomputeRef->glGetVertexProgram()->setProgramParameters(v_params);
	m_pAOcomputeRef->glGetFragmentProgram()->setProgramParameters(f_params);
	glCallList(m_occluders[0]->m_AOMapSetup.handle);
	m_pAOcomputeRef->glRender();

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3,GL_FLOAT,sizeof(GL_COORD_VERTEX),m_refVertex);
	glNormalPointer(GL_FLOAT,sizeof(GL_COORD_VERTEX),m_refNormal);

	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
	pExtensions->glClientActiveTextureARB(GL_TEXTURE2_ARB);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,m_refTexCoords);
	glEnable(GL_TEXTURE_2D);
	
	glDrawArrays(GL_POINTS,0,m_refNbVertex);

	m_pAOcomputeRef->glStop();

#ifdef GL_ARB_texture_rectangle
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
#endif
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	pExtensions->glClientActiveTextureARB(GL_TEXTURE0_ARB);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	m_pAOBuffer->glUnBindDisplay();

	CATCH_GL_ERROR;
}

bool CAmbientOcclusionShader::glSetRefData(GL_COORD_VERTEX* refVertex,GL_COORD_VERTEX* refNormal,GL_TEX_VERTEX* refTexCoords)
{
	m_refVertex = refVertex;
	m_refNormal = refNormal;
	m_refTexCoords = refTexCoords;

	return true;
}

bool CAmbientOcclusionShader::glSetTexCoords(GL_TEX_VERTEX* refTexCoords, unsigned int nbVertex)
{
	m_refTexCoords = refTexCoords;
	m_refNbVertex = nbVertex;
	return true;
}

bool CAmbientOcclusionShader::glSetCoords(GL_COORD_VERTEX* refVertex, unsigned int nbVertex)
{
	m_refVertex = refVertex;
	m_refNbVertex = nbVertex;

	if (refVertex != NULL)
	{
		CTextureFactory &f = CTextureFactory::getDefaultFactory();
		f.getConfig().useTextureResize(false);

		int width = ceil(log(sqrt((float)nbVertex)) / log(2.0));
		int height = width = pow(2.0,width);

		CTextureObject *T = f.glCreateRectangleTexture(	CTextureObject::CGL_COLOR_FLOAT32_ALPHA,
														CTextureObject::CGL_OPAQUE,
														CTextureObject::CGL_UNFILTERED);
		// Should this case still be supported ?
		// with no texture rectangle, it is rather laborious
		if (T == NULL)
		{
			T = f.glCreateTexture(	CTextureObject::CGL_COLOR_FLOAT32_ALPHA,
									CTextureObject::CGL_OPAQUE,
									CTextureObject::CGL_UNFILTERED);
		}

		T->setSize(width,height);
		T->allocateTexels(CTextureObject::CGL_COLOR_FLOAT32_ALPHA);
		float *texels = T->getFloatTexels();
		memcpy(texels,refVertex,nbVertex*sizeof(GL_COORD_VERTEX));

		f.glLoadTexture(T,".buffer");
		m_pVertexMap = T;

		CATCH_GL_ERROR;
		return true;
	}
	else
		return false;
}

bool CAmbientOcclusionShader::glSetNormals(GL_COORD_VERTEX* refNormal, unsigned int nbVertex)
{
	m_refNormal = refNormal;
	m_refNbVertex = nbVertex;

	if (refNormal != NULL)
	{
		CTextureFactory &f = CTextureFactory::getDefaultFactory();
		f.getConfig().useTextureResize(false);

		int width = ceil(log(sqrt((float)nbVertex)) / log(2.0));
		int height = width = pow(2.0,width);

		CTextureObject *T = f.glCreateRectangleTexture(	CTextureObject::CGL_COLOR_FLOAT32_ALPHA,
														CTextureObject::CGL_OPAQUE,
														CTextureObject::CGL_UNFILTERED);

		// Should this case still be supported ?
		// with no texture rectangle, it is rather laborious
		if (T == NULL)
		{
			T = f.glCreateTexture(	CTextureObject::CGL_COLOR_FLOAT32_ALPHA,
									CTextureObject::CGL_OPAQUE,
									CTextureObject::CGL_UNFILTERED);
		}

		T->setSize(width,height);
		T->allocateTexels(CTextureObject::CGL_COLOR_FLOAT32_ALPHA);
		float *texels = T->getFloatTexels();
		memcpy(texels,refNormal,nbVertex*4*sizeof(float));

		f.glLoadTexture(T,".buffer");
		m_pNormalMap = T;

		CATCH_GL_ERROR;
		return true;
	}
	else
		return false;
}
