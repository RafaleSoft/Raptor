// DOT3BumppedGeometry.cpp: implementation of the CDOT3BumppedGeometry class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys\CodeGeneration.h"



#if !defined(AFX_DOT3BUMPPEDGEOMETRY_H__C1ED14FB_CABA_4F04_A7AE_433BB692709C)
	#include "DOT3BumppedGeometry.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_TEXTUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "GLHierarchy/TextureUnitSetup.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif


RAPTOR_NAMESPACE


CDOT3BumppedGeometry::CDOT3BumppedGeometry()
	:bumpDiffusePx(NULL),bumpSpecularPx(NULL)
{
}


CDOT3BumppedGeometry::~CDOT3BumppedGeometry()
{
	if (NULL != bumpDiffusePx)	delete[] bumpDiffusePx;
	if (NULL != bumpSpecularPx)	delete[] bumpSpecularPx;
}


void CDOT3BumppedGeometry::allocBumpColors()
{
	if (!m_pBumpShader->hasVertexShader())
	{
		if (bumpDiffusePx == NULL)
			bumpDiffusePx = new GLubyte[4 * m_nbVertex];
		if (bumpSpecularPx == NULL)
			bumpSpecularPx = new GLubyte[4 * m_nbVertex];
	}
}


CBumppedGeometry& CDOT3BumppedGeometry::operator=(const CDOT3BumppedGeometry &geo)
{
	CBumppedGeometry::operator =(geo);

	if (bumpDiffusePx != NULL) delete[] bumpDiffusePx;
	if (bumpSpecularPx != NULL) delete[] bumpSpecularPx;

	return *this;
}


void CDOT3BumppedGeometry::setRenderingModel(const CRenderingModel& model)
{
	if (model.hasModel(CRenderingModel::CGL_DOT3_BUMP))
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		if (!Raptor::glIsExtensionSupported("GL_ARB_texture_env_dot3"))
		{
			Raptor::GetErrorManager()->generateRaptorError(CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
				CRaptorErrorManager::RAPTOR_WARNING,
				CRaptorMessages::ID_NO_GPU_DOT3);
		}
#endif
	}
}


void CBumppedGeometry::glSetTextureUnits(void)
{
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
}


unsigned int CDOT3BumppedGeometry::glUpdateLightPosition(void)
{
	CBumppedGeometry::glUpdateLightPosition();

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
}


void CDOT3BumppedGeometry::glRender()
{
	GLint func;
	glGetIntegerv(GL_DEPTH_FUNC, &func);
	GLboolean light;
	glGetBooleanv(GL_LIGHTING, &light);
	glDisable(GL_LIGHTING);

	//	1st pass to render diffuse and z-buffer
	glBlendFunc(GL_ONE, GL_ZERO);

	glEnableClientState(GL_VERTEX_ARRAY);
#if defined(DATA_EXTENDED)
	glVertexPointer(3, GL_FLOAT, sizeof(GL_VERTEX_DATA), &geometry[0].vertex);
#elif defined(DATA_PACKED)
	glVertexPointer(3, GL_FLOAT, sizeof(GL_COORD_VERTEX), vertex);
#endif

	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
#if defined(GL_EXT_compiled_vertex_array)
	if (pExtensions->glLockArraysEXT != NULL)
		pExtensions->glLockArraysEXT(0, m_nbVertex);
#endif

	//	diffuse
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, bumpDiffusePx);

	m_pBumpShader->glRenderTexture();

#if defined(GL_ARB_multitexture)
	if (pExtensions->glClientActiveTextureARB != NULL)
	{
		pExtensions->glClientActiveTextureARB(GL_TEXTURE0_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#if defined(DATA_EXTENDED)
		glTexCoordPointer(2, GL_FLOAT, sizeof(GL_VERTEX_DATA), &geometry[0].texCoord0);
#elif defined(DATA_PACKED)
		glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
#endif
		pExtensions->glClientActiveTextureARB(GL_TEXTURE1_ARB);
	}
#endif
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#if defined(DATA_EXTENDED)
	glTexCoordPointer(2, GL_FLOAT, sizeof(GL_VERTEX_DATA), &geometry[0].texCoord0);
#elif defined(DATA_PACKED)
	glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
#endif

	glDrawElements(GL_TRIANGLES, 3 * m_nbPolys, GL_UNSIGNED_SHORT, polys);

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
	glBlendFunc(GL_ONE, GL_ONE);
	glDepthFunc(GL_EQUAL);

	//	specular

	glColorPointer(4, GL_UNSIGNED_BYTE, 0, bumpSpecularPx);
	normalMap->glRender();

#if defined(GL_ARB_texture_env_dot3)
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_DOT3_RGB_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PRIMARY_COLOR_ARB);
#endif
	glDrawElements(GL_TRIANGLES, 3 * m_nbPolys, GL_UNSIGNED_SHORT, polys);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

#if defined(GL_EXT_compiled_vertex_array)
	if (pExtensions->glUnlockArraysEXT != NULL)
		pExtensions->glUnlockArraysEXT();
#endif

	glDepthFunc(func);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (light)
		glEnable(GL_LIGHTING);

	CATCH_GL_ERROR
}


void CDOT3BumppedGeometry::setLightPositionDOT3(void)
{
	//	inverse transform
	CGenericVector<float>	diffuse;
	GL_COORD_VERTEX	specular;
	CGenericVector<float>	viewer;

	float m_fShininess = normalMap->getTransparency();
	float d, d2;
	int index = 0;

	for (unsigned int i = 0; i<m_nbVertex; i++)
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
				bumpDiffusePx[index + 1] = ((unsigned char)(L.y + 128));
				bumpDiffusePx[index + 2] = ((unsigned char)(L.z + 128));

				specular.x = 0.5f * (viewer.X() + diffuse.X());
				specular.y = 0.5f * (viewer.Y() + diffuse.Y());
				specular.z = 0.5f * (viewer.Z() + diffuse.Z());

				L.z = 0.5f * m_fShininess * (specular.x * NORMAL(i).x + specular.y * NORMAL(i).y + specular.z * NORMAL(i).z);
				L.x = 0.5f * m_fShininess * (specular.x * TANGENT(i).x + specular.y * TANGENT(i).y + specular.z * TANGENT(i).z);
				L.y = 0.5f * m_fShininess * (specular.x * BINORMAL(i).x + specular.y * BINORMAL(i).y + specular.z * BINORMAL(i).z);

				bumpSpecularPx[index] = ((unsigned char)(L.x + 128));
				bumpSpecularPx[index + 1] = ((unsigned char)(L.y + 128));
				bumpSpecularPx[index + 2] = ((unsigned char)(L.z + 128));
			}
		}

		index += 4;
	}
}

