/***************************************************************************/
/*                                                                         */
/*  AmbientOcclusionShader.cpp                                             */
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

#if !defined(AFX_AMBIENTOCCLUSIONSHADER_H__FA8234C4_82B1_49D3_ABAA_7FCE45EDDCAD__INCLUDED_)
	#include "AmbientOcclusionShader.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "GLHierarchy/TextureSet.h"
#endif
#if !defined(AFX_IRENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/IRenderingProperties.h"
#endif
#if !defined(AFX_TEXUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "GLHierarchy/TextureUnitSetup.h"
#endif
#if !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)
	#include "GLHierarchy/ShaderProgram.h"
#endif
#if !defined(AFX_VERTEXSHADER_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
	#include "GLHierarchy/VertexShader.h"
#endif
#if !defined(AFX_FRAGMENTSHADER_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
    #include "GLHierarchy/FragmentShader.h"
#endif
#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/3DEngine.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "GLHierarchy/OpenGLShaderStage.h"
#endif


RAPTOR_NAMESPACE


CAmbientOcclusionShader::CAmbientOcclusionShader(const std::string& name)
	:CShader(name),
	m_refNbVertex(0), m_refVertex(NULL), m_refNormal(NULL), m_refTexCoords(NULL),
	m_pVertexMap(NULL),m_pNormalMap(NULL),m_pAOMap(NULL),m_pAOBuffer(NULL),
	m_pAOcomputeRef(NULL), m_tranformRef(NULL)
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
	CTextureObject *T = f.glCreateTexture(	ITextureObject::CGL_COLOR_FLOAT32_ALPHA,
											CTextureObject::CGL_OPAQUE,
											ITextureObject::CGL_BILINEAR);
	T->glvkUpdateClamping(ITextureObject::CGL_EDGECLAMP);
	f.glResizeTexture(T,m_pVertexMap->getWidth(),m_pVertexMap->getHeight());

	CTextureSet *pOutputTextures = new CTextureSet("Filter Output Render Textures");
	pOutputTextures->addTexture(T);
	T->releaseReference();
	m_pAOMap = T;

	CRaptorDisplayConfig cfg;
	cfg.width = m_pVertexMap->getWidth();
	cfg.height = m_pVertexMap->getHeight();
	cfg.acceleration = CRaptorDisplayConfig::HARDWARE;
	cfg.display_mode =	CGL_FLOAT;
	cfg.bind_to_texture = true;
	cfg.renderer = CRaptorDisplayConfig::RENDER_BUFFER;

    m_pAOBuffer = Raptor::glCreateDisplay(cfg);
	IRenderingProperties &rp = m_pAOBuffer->getRenderingProperties();
	rp.setTexturing(IRenderingProperties::ENABLE);
	rp.setCullFace(IRenderingProperties::DISABLE);
	rp.setDepthTest(IRenderingProperties::DISABLE);
	rp.setLighting(IRenderingProperties::DISABLE);
    rp.clear(CGL_NULL);
    m_pAOBuffer->setViewPoint(NULL);
	m_pAOBuffer->glvkBindDisplay(*pOutputTextures);

	m_pAOcomputeRef = CShader::getShader("AOCOMPUTE_SHADER").glClone("AO_SHADER");
	CTextureUnitSetup *AOdata = glGetTextureUnitsSetup();
	AOdata->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_0,true);
	AOdata->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_1,true);
	AOdata->setDiffuseMap(m_pVertexMap);
	AOdata->setNormalMap(m_pNormalMap);
	m_AOMapSetup = AOdata->glBuildSetup();

	CATCH_GL_ERROR;

	if (!Raptor::glIsExtensionSupported(GL_ARB_TEXTURE_RECTANGLE_EXTENSION_NAME))
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

	if (IRenderingProperties::GetCurrentProperties()->getCurrentTexturing() == IRenderingProperties::ENABLE)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glClientActiveTextureARB(GL_TEXTURE2_ARB);

		// This is responsibility of a geometry
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,GL_FLOAT,0,m_refTexCoords);

		pExtensions->glActiveTextureARB(GL_TEXTURE2_ARB);
		glEnable(GL_TEXTURE_2D);
		m_pAOMap->glvkRender();
		/*
		CTextureFactory &f = CTextureFactory::getDefaultFactory();
		f.glExportTexture(m_pAOMap, "AO_map.jpg");
		*/
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
		pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
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

	CProgramParameters ao_params;
	ao_params.addParameter("vertexMat",vertexMat);
	ao_params.addParameter("normalMat",normalMat);
	ao_params.addParameter("posMap",CTextureUnitSetup::IMAGE_UNIT_0);
	ao_params.addParameter("normalMap",CTextureUnitSetup::IMAGE_UNIT_1);
	ao_params.addParameter("numRows",GL_COORD_VERTEX(m_occluders[0]->m_refNbVertex/64,0,0,0));

	RAPTOR_HANDLE noDevice;
	m_pAOBuffer->glvkBindDisplay(noDevice);

#ifdef GL_ARB_texture_rectangle
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
#endif
	m_pAOcomputeRef->glGetOpenGLShader()->setProgramParameters(ao_params);
	glCallList(m_occluders[0]->m_AOMapSetup.handle());
	m_pAOcomputeRef->glRender();

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3,GL_FLOAT,sizeof(GL_COORD_VERTEX),m_refVertex);
	glNormalPointer(GL_FLOAT,sizeof(GL_COORD_VERTEX),m_refNormal);

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
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

	m_pAOBuffer->glvkUnBindDisplay();

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

		CTextureObject *T = f.glCreateRectangleTexture(	ITextureObject::CGL_COLOR_FLOAT32_ALPHA,
														CTextureObject::CGL_OPAQUE,
														ITextureObject::CGL_UNFILTERED);
		// Should this case still be supported ?
		// with no texture rectangle, it is rather laborious
		if (T == NULL)
		{
			T = f.glCreateTexture(	ITextureObject::CGL_COLOR_FLOAT32_ALPHA,
									CTextureObject::CGL_OPAQUE,
									ITextureObject::CGL_UNFILTERED);
		}

		CImage coords;
		coords.allocatePixels(width, height, CImage::CGL_COLOR_FLOAT32_ALPHA);
		float *pixels = coords.getFloatPixels();
		memcpy(pixels, refVertex, nbVertex*sizeof(GL_COORD_VERTEX));

		f.glLoadTexture(T, coords);
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

		CTextureObject *T = f.glCreateRectangleTexture(	ITextureObject::CGL_COLOR_FLOAT32_ALPHA,
														CTextureObject::CGL_OPAQUE,
														ITextureObject::CGL_UNFILTERED);

		// Should this case still be supported ?
		// with no texture rectangle, it is rather laborious
		if (T == NULL)
		{
			T = f.glCreateTexture(	ITextureObject::CGL_COLOR_FLOAT32_ALPHA,
									CTextureObject::CGL_OPAQUE,
									ITextureObject::CGL_UNFILTERED);
		}

		CImage normals;
		normals.allocatePixels(width, height, CImage::CGL_COLOR_FLOAT32_ALPHA);
		float *pixels = normals.getFloatPixels();
		memcpy(pixels,refNormal,nbVertex*4*sizeof(float));

		f.glLoadTexture(T, normals);
		m_pNormalMap = T;

		CATCH_GL_ERROR;
		return true;
	}
	else
		return false;
}
