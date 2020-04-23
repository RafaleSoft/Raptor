// DOT3BumppedGeometry.cpp: implementation of the CDOT3BumppedGeometry class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys\CodeGeneration.h"



#if !defined(AFX_DOT3BUMPPEDGEOMETRY_H__C1ED14FB_CABA_4F04_A7AE_433BB692709C)
	#include "DOT3BumppedGeometry.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_EMBMSHADER_H__99A5AF45_D5C7_4F43_851C_A31FC52DB237__INCLUDED_)
	#include "Subsys/EMBMShader.h"
#endif
#if !defined(AFX_TEXTUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "GLHierarchy/TextureUnitSetup.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#if !defined(AFX_3DENGINEMATRIX_H__6CD1110E_1174_4f38_A452_30FB312022D0__INCLUDED_)
	#include "Engine/3DEngineMatrix.h"
#endif
#if !defined(AFX_SIMPLELIGHTOBSERVER_H__238FC166_A3BC_4D77_8FD4_0A42DB45280F__INCLUDED_)
	#include "SimpleLightObserver.h"
#endif
#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "GLHierarchy/Light.h"
#endif

RAPTOR_NAMESPACE

CSimpleLightObserver* CDOT3BumppedGeometry::m_pObserver = NULL;


CDOT3BumppedGeometry::CDOT3BumppedGeometry(const std::string& name)
	:CBumppedGeometry(name),
	bumpDiffusePx(NULL),bumpSpecularPx(NULL)
{
	if (m_pObserver == NULL)
		m_pObserver = new CSimpleLightObserver();
	else
		m_pObserver->addReference();
}


CDOT3BumppedGeometry::~CDOT3BumppedGeometry()
{
	if (NULL != bumpDiffusePx)	delete[] bumpDiffusePx;
	if (NULL != bumpSpecularPx)	delete[] bumpSpecularPx;

	if (NULL != m_pObserver)
	{
		bool lastObject = (m_pObserver->getRefCount() == 1);
		m_pObserver->releaseReference();
		if (lastObject)
			m_pObserver = NULL;
	}
}


CBumppedGeometry& CDOT3BumppedGeometry::operator=(const CDOT3BumppedGeometry &geo)
{
	CBumppedGeometry::operator =(geo);

	if (bumpDiffusePx != NULL) delete[] bumpDiffusePx;
	if (bumpSpecularPx != NULL) delete[] bumpSpecularPx;

	return *this;
}


//void CDOT3BumppedGeometry::setRenderingModel(const CRenderingModel& model)
void CDOT3BumppedGeometry::setRenderingModel(CGeometry::RENDERING_MODEL model)
{
	CBumppedGeometry::setRenderingModel(model);

	//	Rendering requires separate specular & seconday color
	if (!Raptor::glIsExtensionSupported(GL_EXT_SECONDARY_COLOR_EXTENSION_NAME) ||
		!Raptor::glIsExtensionSupported(GL_ARB_TEXTURE_ENV_DOT3_EXTENSION_NAME))
	{
		Raptor::GetErrorManager()->generateRaptorError(	CBumppedGeometry::CBumppedGeometryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_NO_GPU_PROGRAM);
		return;
	}

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
}


unsigned int CDOT3BumppedGeometry::glUpdateLightPosition(void)
{
	unsigned int numLights = 0;
	C3DEngineMatrix T;

	glGetTransposeFloatv(GL_MODELVIEW_MATRIX, T);

	GL_COORD_VERTEX center;
	getCenter(center);
	CGenericVector<float> x(center.x, center.y, center.z, 1.0f);
	x *= T;
	vector<CLight*> lights = m_pObserver->sortLights(x);
	if (lights.size() < 1)
		return numLights;
	CLight *pMainLight = lights[0];

	V.x = -(T[0] * T[3] + T[4] * T[7] + T[8] * T[11]);
	V.y = -(T[1] * T[3] + T[5] * T[7] + T[9] * T[11]);
	V.z = -(T[2] * T[3] + T[6] * T[7] + T[10] * T[11]);

	T.Inverse();

	numLights++;
	X = T * pMainLight->getLightEyePosition();


	GL_COORD_VERTEX att = pMainLight->getSpotParams();
	A.x = att.z;
	A.y = att.y;
	A.z = att.x;
	A.h = 0.0f;
	S = pMainLight->getSpecular();

	/*
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
	*/
	if (bumpDiffusePx == NULL)
	{
		if (bumpDiffusePx == NULL)
			bumpDiffusePx = new GLubyte[4 * m_nbVertex];
		if (bumpSpecularPx == NULL)
			bumpSpecularPx = new GLubyte[4 * m_nbVertex];
	}

#ifdef RAPTOR_SSE_CODE_GENERATION
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	if (instance.forceSSE)
		setLightPositionDOT3SSE();
	else
		setLightPositionDOT3();
#else
	setLightPositionDOT3();
#endif

	return numLights;
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

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
#if defined(GL_EXT_compiled_vertex_array)
	if (pExtensions->glLockArraysEXT != NULL)
		pExtensions->glLockArraysEXT(0, m_nbVertex);
#endif

	//	diffuse
	//	obsolete : remove generic attributes binding
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
	//	obsolete : remove generic attributes binding
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, bumpSpecularPx);
	normalMap->glvkRender();

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


#if defined(RAPTOR_SSE_CODE_GENERATION) && !defined(_WIN64)

static CSSERegister0 xmm0;
static CSSERegister1 xmm1;
static CSSERegister2 xmm2;
static CSSERegister3 xmm3;
static CSSERegister4 xmm4;
static CSSERegister5 xmm5;
static CSSERegister6 xmm6;
static CSSERegister7 xmm7;

#endif

static float half[4] = { 0.5f, 0.5f, 0.5f, 0.5f };


void CDOT3BumppedGeometry::setLightPositionDOT3SSE(void)
{
#if defined(RAPTOR_SSE_CODE_GENERATION) && !defined(_WIN64)
	//	inverse transform
	GL_COORD_VERTEX	diffuse;
	GL_COORD_VERTEX	specular;
	GL_COORD_VERTEX	viewer;

	float m_fShininess = normalMap->getTransparency();

	float d, d2;
	//	here, this is slower for 2%, so ...
	//float half_shininess = 0.5f * m_fShininess;
	int index = 0;

	GL_COORD_VERTEX L;
	xmm7 = X.vector();
	xmm6 = V;

	for (unsigned int i = 0; i<m_nbVertex; i++)
	{
		//	Light vector
		xmm4 = xmm7;
		xmm5 = VERTEX(i);
		xmm4 -= xmm5;

		//	normalize
		xmm0 = xmm4;
		xmm0 *= xmm4;
		xmm1 = xmm0;
		xmm2 = xmm0;
		sse_shufps(XMM1_XMM0, SSE_R4_R1_R3_R2)
		sse_shufps(XMM2_XMM0, SSE_R4_R2_R1_R3)
		xmm1 += xmm0;
		xmm1 += xmm2;
		sse_rsqrtps(XMM2_XMM1)
		xmm4 *= xmm2;
		xmm4.Save(diffuse);

		d = diffuse.x * NORMAL(i).x + diffuse.y * NORMAL(i).y + diffuse.z * NORMAL(i).z;

		// 1st test : no light
		if (d < 0)
		{
			*((unsigned int*)&bumpDiffusePx[index]) = 0xFF808080;
			*((unsigned int*)&bumpSpecularPx[index]) = 0xFF808080;
		}
		else
		{
			//	viewer vector
			xmm3 = xmm6;
			xmm3 -= xmm5;

			// normalize
			xmm0 = xmm3;
			xmm0 *= xmm3;
			xmm1 = xmm0;
			xmm2 = xmm0;
			sse_shufps(XMM1_XMM0, SSE_R4_R1_R3_R2)
			sse_shufps(XMM2_XMM0, SSE_R4_R2_R1_R3)
			xmm1 += xmm0;
			xmm1 += xmm2;
			sse_rsqrtps(XMM2_XMM1)
			xmm3 *= xmm2;
			xmm3.Save(viewer);

			d2 = viewer.x * NORMAL(i).x + viewer.y * NORMAL(i).y + viewer.z * NORMAL(i).z;

			// 2nd test : no view
			if (d2 < 0)
			{
				*((unsigned int*)&bumpDiffusePx[index]) = 0xFF808080;
				*((unsigned int*)&bumpSpecularPx[index]) = 0xFF808080;
			}
			else
			{
				xmm3 += xmm4;
				xmm2 = half;
				xmm3 *= xmm2;
				xmm3.Save(specular);

				L.z = 127.0f * d;
				L.x = 127.0f * (diffuse.x * TANGENT(i).x + diffuse.y * TANGENT(i).y + diffuse.z * TANGENT(i).z);
				L.y = 127.0f * (diffuse.x * BINORMAL(i).x + diffuse.y * BINORMAL(i).y + diffuse.z * BINORMAL(i).z);

				bumpDiffusePx[index] = ((unsigned char)(L.x + 128));
				bumpDiffusePx[index + 1] = ((unsigned char)(L.y + 128));
				bumpDiffusePx[index + 2] = ((unsigned char)(L.z + 128));

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
#endif
}

