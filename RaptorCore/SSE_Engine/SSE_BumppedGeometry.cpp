#include "Subsys/CodeGeneration.h"

#ifdef RAPTOR_SSE_CODE_GENERATION

	#if !defined(AFX_BUMPPEDGEOMETRY_H__FDCE89B9_B923_4325_AB0D_A12486C6756D__INCLUDED_)
		#include "GLHierarchy/BumppedGeometry.h"
	#endif
	#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
		#include "Engine/3DEngine.h"
	#endif
	#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
		#include "GLHierarchy/TextureObject.h"
	#endif

RAPTOR_NAMESPACE_BEGIN

static CSSERegister0 xmm0;
static CSSERegister1 xmm1;
static CSSERegister2 xmm2;
static CSSERegister3 xmm3;
static CSSERegister4 xmm4;
static CSSERegister5 xmm5;
static CSSERegister6 xmm6;
static CSSERegister7 xmm7;

static float half[4] = { 0.5f, 0.5f, 0.5f, 0.5f };

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

void CBumppedGeometry::setLightPositionDOT3SSE(void)
{
	//	inverse transform
	GL_COORD_VERTEX	diffuse;
	GL_COORD_VERTEX	specular;
	GL_COORD_VERTEX	viewer;

	float m_fShininess = normalMap->getTransparency();

	float d,d2;
	//	here, this is slower for 2%, so ...
	//float half_shininess = 0.5f * m_fShininess;
	int index = 0;

	GL_COORD_VERTEX L;
	xmm7 = X.vector();
	xmm6 = V;

	for (unsigned int i=0;i<m_nbVertex;i++)
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
		sse_shufps(XMM1_XMM0,SSE_R4_R1_R3_R2)
		sse_shufps(XMM2_XMM0,SSE_R4_R2_R1_R3)
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
			sse_shufps(XMM1_XMM0,SSE_R4_R1_R3_R2)
			sse_shufps(XMM2_XMM0,SSE_R4_R2_R1_R3)
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
				bumpDiffusePx[index+1] = ((unsigned char)(L.y + 128));
				bumpDiffusePx[index+2] = ((unsigned char)(L.z + 128));

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

#endif