// VertexShadersDisplay.cpp: implementation of the CVertexShadersDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "VertexShadersDisplay.h"
#include "GLHierarchy\VertexShader.h"
#include "GLHierarchy\FragmentShader.h"
#include "Engine\3DEngine.h"
#include "Engine\3DScene.h"
#include "Engine\ViewPoint.h"
#include "GLHierarchy\TextureUnitSetup.h"
#include "System\Raptor.h"
#include "GLHierarchy\Shader.h"
#include "GLHierarchy\TextureFactory.h"
#include "GLHierarchy\TextureFactoryConfig.h"
#include "GLHierarchy\TextureObject.h"
#include "GLHierarchy/TextureSet.h"
#include "Engine\GeometricModifier.h"
#include "GLHierarchy\3DSet.h"
#include "GLHierarchy\Light.h"
#include "GLHierarchy\SimpleObject.h"
#include "GLHierarchy\ShadedGeometry.h"
#include "GLHierarchy\RenderingProperties.h"


GL_COORD_VERTEX LPOS;
const int TABLE_SIZE = 256;

string waterShader = 
"!!ARBvp1.0 \
ATTRIB iPos = vertex.position; \
ATTRIB iColor = vertex.color.primary; \
ATTRIB iTexCoord0 = vertex.texcoord; \
PARAM mvp[4] = { state.matrix.mvp }; \
PARAM mvit[4] = { state.matrix.modelview.invtrans }; \
PARAM lpos = program.local[15]; \
PARAM xDirs = program.local[0]; \
PARAM zDirs = program.local[1]; \
PARAM A = program.local[2]; \
PARAM freq = program.local[3]; \
PARAM phase = program.local[4]; \
PARAM Q = program.local[5]; \
PARAM kSinConsts = { 1.0 , -0.16667, 0.00833, -0.0001984 }; \
PARAM kCosConsts = { 1.0 , -0.5, 0.041667, -0.0013889 }; \
PARAM kPis = { 3.14159265 , 6.28318530 ,  0.15915494, 0.5 }; \
TEMP coord; \
TEMP coord2; \
TEMP coord3; \
TEMP coord4; \
TEMP coord5; \
TEMP coord6; \
TEMP coord7; \
TEMP sinus; \
TEMP cosinus; \
OUTPUT oPos = result.position; \
OUTPUT oPCol = result.color.primary; \
OUTPUT oSCol = result.color.secondary; \
OUTPUT oTex0 = result.texcoord[0]; \
OUTPUT oTex1 = result.texcoord[1]; \
OUTPUT oTex2 = result.texcoord[2]; \
OUTPUT oTex3 = result.texcoord[3]; \
OUTPUT oTex4 = result.texcoord[4]; \
#	Calcul de la position de l'onde:		\
	Di . (x,z) * wi + t * phi				\n\
MUL coord, xDirs, iPos.xxxx; \
MAD coord, zDirs, iPos.zzzz, coord; \
MAD coord, coord, freq, phase; \
#	Mod [-pi..pi], car le developpement limite		\
	des sinus et cosinus a l'ordre 7 est correct	\
	entre [-pi..pi]									\n\
ADD coord, kPis.xxxx, coord; \
MUL coord, kPis.zzzz, coord; \
FRC coord, coord; \
MUL coord, kPis.yyyy, coord; \
ADD coord, -kPis.xxxx, coord; \
#	Calcul des cosinus et sinus		\n\
MUL coord2, coord, coord; \
MUL coord3, coord2, coord; \
MUL coord4, coord2, coord2; \
MUL coord5, coord3, coord2; \
MUL coord6, coord3, coord3; \
MUL coord7, coord4, coord3; \
MAD sinus, coord3, kSinConsts.yyyy, coord; \
MAD sinus, coord5, kSinConsts.zzzz, sinus; \
MAD sinus, coord7, kSinConsts.wwww, sinus; \
MAD cosinus, coord2, kCosConsts.yyyy, kCosConsts.xxxx; \
MAD cosinus, coord4, kCosConsts.zzzz, cosinus; \
MAD cosinus, coord6, kCosConsts.wwww , cosinus; \
";

string waterShader2 =
"#	Resultat final						\
	- position							\n\
PARAM factor = { 1.0, 0.0, 1.0, 1.0 }; \
PARAM kNx = program.local[6]; \
PARAM kNy = program.local[7]; \
PARAM kNz = program.local[8]; \
PARAM kTx = program.local[9]; \
PARAM kTy = program.local[10]; \
PARAM kTz = program.local[11]; \
PARAM kBx = program.local[12]; \
PARAM kBy = program.local[13]; \
PARAM kBz = program.local[14]; \
TEMP waves; \
MUL waves, cosinus, Q; \
DP4 coord.x, waves, xDirs; \
DP4 coord.y, A, sinus; \
DP4 coord.z, waves, zDirs; \
MOV coord.w, factor.y; \
MUL waves, iPos, factor; \
ADD coord, coord, waves; \
DP4 oPos.x , mvp[0] , coord; \
DP4 oPos.y , mvp[1] , coord; \
DP4 oPos.z , mvp[2] , coord; \
DP4 oPos.w , mvp[3], coord; \
MOV oTex0, iTexCoord0; \
#	- normales							\n\
TEMP normal;\
TEMP tangent; \
TEMP binormal; \
TEMP diffuse; \
TEMP specular; \
MOV waves, factor.yyxw; \
DP4 tangent.x, kTx, sinus; \
DP4 tangent.y, kTy, cosinus; \
DP4 tangent.z, kTz, sinus; \
SUB tangent, waves, tangent; \
DP3 tangent.w, tangent, tangent; \
RSQ tangent.w, tangent.w; \
MUL tangent.xyz, tangent.w, tangent; \
MOV waves, factor.xyyw; \
DP4 binormal.x, kBx, sinus; \
DP4 binormal.y, kBy, cosinus; \
DP4 binormal.z, kBz, sinus; \
SUB binormal, waves, binormal; \
DP3 binormal.w, binormal, binormal; \
RSQ binormal.w, binormal.w; \
MUL binormal.xyz, binormal.w, binormal; \
MOV waves,factor.yxyw; \
DP4 normal.x, kNx, cosinus; \
DP4 normal.y, kNy, sinus; \
DP4 normal.z, kNz, cosinus; \
SUB normal, waves, normal; \
DP3 normal.w, normal, normal; \
RSQ normal.w, normal.w; \
MUL normal.xyz, normal.w, normal; \
";


string waterShader3 =
"#	- couleur							\n\
PARAM half = { 0.5, 0.5, 0.5, 0.0 }; \
SUB diffuse, lpos, coord; \
DP3 diffuse.w, diffuse, diffuse; \
RSQ diffuse.w, diffuse.w; \
MUL diffuse.xyz, diffuse, diffuse.w; \
SUB specular, mvit[3], coord; \
DP3 specular.w, specular, specular; \
RSQ specular.w, specular.w; \
MUL specular.xyz, specular, specular.w; \
ADD specular, specular, diffuse; \
DP3 specular.w, specular, specular; \
RSQ specular.w, specular.w; \
MUL specular.xyz, specular, specular.w; \
DP3 waves.x, diffuse, binormal; \
DP3 waves.y, diffuse, tangent; \
DP3 waves.z, diffuse, normal; \
MAD oPCol, waves, half, half; \
DP3 waves.x, specular, binormal; \
DP3 waves.y, specular, tangent; \
DP3 waves.z, specular, normal; \
MAD oSCol, waves, half, half; \
#	- texCoord pour EMBM				\n\
TEMP eyePos; \
SUB eyePos, mvit[3], coord;\
DP3 eyePos.w, eyePos, eyePos;\
RSQ eyePos.w, eyePos.w;\
MUL eyePos.xyz, eyePos.w, eyePos;\
MOV oTex1.x, tangent.x;\
MOV oTex1.y, binormal.x;\
MOV oTex1.z, normal.x;\
MOV oTex2.x, tangent.y;\
MOV oTex2.y, binormal.y;\
MOV oTex2.z, normal.y;\
MOV oTex3.x, tangent.z;\
MOV oTex3.y, binormal.z;\
MOV oTex3.z, normal.z;\
MOV oTex4, eyePos; \
END\
";

string waterFragments =
"!!ARBfp1.0 \
ATTRIB iTex = fragment.texcoord[0]; \
ATTRIB iTex1 = fragment.texcoord[1]; \
ATTRIB iTex2 = fragment.texcoord[2]; \
ATTRIB iTex3 = fragment.texcoord[3]; \
ATTRIB iEyePos = fragment.texcoord[4]; \
ATTRIB iDiffuse = fragment.color.primary; \
ATTRIB iSpecular = fragment.color.secondary; \
OUTPUT finalColor = result.color; \
PARAM two = { 2.0 , 2.0, 2.0, 0.0 }; \
PARAM one = { -1.0 , -1.0, -1.0, 1.0 }; \
PARAM reflect = { 0.4, 0.4, 0.4, 0 }; \
PARAM shading = program.local[0]; \
PARAM shininess = program.local[1]; \
TEMP color; \
TEMP normal; \
TEMP diffuse; \
TEMP specular; \
TEMP environment; \
TEMP U; \
TEMP R;\
TEMP exponent; \
TEX color, iTex , texture[0], 2D ; \
TEX normal, iTex , texture[1], 2D ; \
MAD normal, normal, two, one; \
DP3 U.x, iTex1, normal; \
DP3 U.y, iTex2, normal; \
DP3 U.z, iTex3, normal; \
DP3 R, U, iEyePos;\
MUL R, R, U;\
MUL R, R, two;\
SUB R, R, iEyePos;\
MOV R.w, one.w; \
TEX environment, R , texture[3] , CUBE ; \
MAD diffuse, iDiffuse, two, one; \
DP3 diffuse, diffuse, normal; \
LRP diffuse, color.wwww, color, diffuse; \
MAD_SAT diffuse, diffuse, shading.yyyy, shading.xxxx; \
MAD_SAT diffuse, environment, reflect, diffuse; \
MAD specular, iSpecular, two, one; \
DP3 specular, specular, normal; \
POW specular, specular.x, shininess.x; \
MAD_SAT finalColor, specular, shading.z, diffuse; \
MOV finalColor.w, shading.w; \
END" ;


string waterShader4 = 
"!!ARBvp1.0 \
ATTRIB iPos = vertex.position; \
ATTRIB iTexCoord0 = vertex.texcoord; \
PARAM time = program.local[0]; \
PARAM mvp[4] = { state.matrix.mvp }; \
PARAM dir0 = program.local[1]; \
PARAM dir1 = program.local[2]; \
PARAM dir2 = program.local[3]; \
PARAM dir3 = program.local[4]; \
PARAM dir4 = program.local[5]; \
PARAM dir5 = program.local[6]; \
PARAM dir6 = program.local[7]; \
PARAM dir7 = program.local[8]; \
PARAM freq123 = program.local[9]; \
PARAM freq456 = program.local[10]; \
PARAM freq789 = program.local[11]; \
TEMP tmp; \
OUTPUT oPos = result.position; \
OUTPUT oTex0 = result.texcoord[0]; \
OUTPUT oTex1 = result.texcoord[1]; \
OUTPUT oTex2 = result.texcoord[2]; \
OUTPUT oTex3 = result.texcoord[3]; \
OUTPUT oTex4 = result.texcoord[4]; \
OUTPUT oTex5 = result.texcoord[5]; \
OUTPUT oTex6 = result.texcoord[6]; \
OUTPUT oTex7 = result.texcoord[7]; \
DP4 oPos.x , mvp[0] , iPos; \
DP4 oPos.y , mvp[1] , iPos; \
DP4 oPos.z , mvp[2] , iPos; \
DP4 oPos.w , mvp[3], iPos; \
MOV tmp, dir0.zwxy; \
DP3 tmp.x, iTexCoord0, dir0.xyxx; \
MAD oTex0, tmp, freq123.xxww, time.xxww; \
MOV tmp, dir1.zwxy; \
DP3 tmp.x, iTexCoord0, dir1.xyxx; \
MAD oTex1, tmp, freq123.yyww, time.yyww; \
MOV tmp, dir2.zwxy; \
DP3 tmp.x, iTexCoord0, dir2.xyxx; \
MAD oTex2, tmp, freq123.zzww, time.zzww; \
MOV tmp, dir3.zwxy; \
DP3 tmp.x, iTexCoord0, dir3.xyxx; \
MAD oTex3, tmp, freq456.xxww, time.xxww; \
MOV tmp, dir4.zwxy; \
DP3 tmp.x, iTexCoord0, dir4.xyxx; \
MAD oTex4, tmp, freq456.yyww, time.yyww; \
MOV tmp, dir5.zwxy; \
DP3 tmp.x, iTexCoord0, dir5.xyxx; \
MAD oTex5, tmp, freq456.zzww, time.zzww; \
MOV tmp, dir6.zwxy; \
DP3 tmp.x, iTexCoord0, dir6.xyxx; \
MAD oTex6, tmp, freq789.xxww, time.xxww; \
MOV tmp, dir7.zwxy; \
DP3 tmp.x, iTexCoord0, dir7.xyxx; \
MAD oTex7, tmp, freq789.yyww, time.yyww; \
END";

string waterFragments2 =
"!!ARBfp1.0 \
ATTRIB iTex0 = fragment.texcoord[0]; \
ATTRIB iTex1 = fragment.texcoord[1]; \
ATTRIB iTex2 = fragment.texcoord[2]; \
ATTRIB iTex3 = fragment.texcoord[3]; \
ATTRIB iTex4 = fragment.texcoord[4]; \
ATTRIB iTex5 = fragment.texcoord[5]; \
ATTRIB iTex6 = fragment.texcoord[6]; \
ATTRIB iTex7 = fragment.texcoord[7]; \
OUTPUT finalColor = result.color; \
PARAM half = { 0.5, 1.0, 0.0, 1.0 }; \
PARAM waves = { -0.125, -0.125, 1.0, 0.0 }; \
TEMP dx; \
TEMP color; \
MOV color, half.zzzz; \
TEX dx, iTex0.xxxx , texture[0], 2D ; \
ADD dx, dx, -half.xxzz; \
MAD color, iTex0.zwww, dx.yyww, color; \
TEX dx, iTex1.xxxx , texture[0], 2D ; \
ADD dx, dx, -half.xxzz; \
MAD color, iTex1.zwww, dx.yyww, color; \
TEX dx, iTex2.xxxx , texture[0], 2D ; \
ADD dx, dx, -half.xxzz; \
MAD color, iTex2.zwww, dx.yyww, color; \
TEX dx, iTex3.xxxx , texture[0], 2D ; \
ADD dx, dx, -half.xxzz; \
MAD color, iTex3.zwww, dx.yyww, color; \
TEX dx, iTex4.xxxx , texture[0], 2D ; \
ADD dx, dx, -half.xxzz; \
MAD color, iTex4.zwww, dx.yyww, color; \
TEX dx, iTex5.xxxx , texture[0], 2D ; \
ADD dx, dx, -half.xxzz; \
MAD color, iTex5.zwww, dx.yyww, color; \
TEX dx, iTex6.xxxx , texture[0], 2D ; \
ADD dx, dx, -half.xxzz; \
MAD color, iTex6.zwww, dx.yyww, color; \
TEX dx, iTex7.xxxx , texture[0], 2D ; \
ADD dx, dx, -half.xxzz; \
MAD color, iTex7.zwww, dx.yyww, color; \
MUL color, color, waves; \
MOV color.zw, half.y; \
DP3 color.w, color, color; \
RSQ color.w, color.w; \
MUL color, color, color.w; \
MAD finalColor, color, half.xxxz, half.xxxw; \
END" ;


class See : public CShadedGeometry
{
public:
	See(float width,float height,int hcels,int vcels)
	{
		float orgx = -width * 0.5f;
		float orgy = -height * 0.5f;
		float stepx = width / hcels;
		float stepy = height / vcels;

		unsigned short *m_indexes = new unsigned short[2 * hcels * vcels];

		int pos = 0;
		memset(m_indexes,0,2*hcels*vcels*sizeof(unsigned short));
		for (int k=0;k<vcels-2;k+=2)
		{
            int i=0;
			for (i=0;i<hcels;i++)
				for (int j=k+1;j>=k;j--)
				{
					m_indexes[pos++] = hcels * j + i;
				}
			for (i=hcels-1;i>=0;i--)
				for (int j=k+1;j<k+3;j++)
				{
					m_indexes[pos++] = hcels * j + i;
				}
		}

		glSetVertices(hcels*vcels);
		glSetTexCoords(hcels*vcels);

		glLockData();

		for (int j=0;j<vcels;j++)
			for (int i=0;i<hcels;i++)
			{
				addVertex(orgx+i*stepx,0,-orgy-j*stepy,1.0f);
				setTexCoord(i+j*hcels, 10 * (float)i / hcels , 10 * (float)j / vcels);
				setNormal(i+j*hcels,0.0f,1.0f,0.0f,1.0f);
			}

		glUnLockData();

		CGeometryPrimitive*	gp = createPrimitive(CGeometryPrimitive::TRIANGLE_STRIP);
		gp->setIndexes(pos,m_indexes);
		delete [] m_indexes;
	}

	virtual ~See() {};
};

class ShaderModifier : public CModifier
{
public:
	ShaderModifier(CShader *shader)
		:CModifier(CModifier::CGL_TIME_CONSTANT,0,0,0,0,NULL,CModifier::CModifierClassID::GetClassId()),
		m_pShader(shader)
    { };

	virtual ~ShaderModifier() {};
	virtual MODIFIER_TYPE getType(void) const
	{
		return CModifier::CGL_GEOMETRIC_MODIFIER;
	};

	virtual void __fastcall deltaTime(float dt)
	{
		float T = GetTime();
	
		float sharpness = 1.0f; 

		GL_COORD_VERTEX xDir(0.0f, 0.866f, -0.866f, 0.0f);
		GL_COORD_VERTEX zDir(-1.0f, -0.5f, -0.5f, -1.0f);
		GL_COORD_VERTEX A(10.0, 6.0, 4.0, 2.0);
		GL_COORD_VERTEX L(200.0f, 250.0f, 300.0f, 350.0f);
		GL_COORD_VERTEX freq(	2 * PI / L.x, 
								2 * PI / L.y, 
								2 * PI / L.z, 
								2 * PI / L.z);
		GL_COORD_VERTEX S(80,60,50,40);

		GL_COORD_VERTEX phase(	S.x * T * freq.x, 
								S.y * T * freq.y,
								S.z * T * freq.z,
								S.h * T * freq.h);

		// Qi = 1 / ( wi . Ai )
		// Sum(Qi.wi.Ai) are slightly > 1 to create a small loop on crest, more funny.
		GL_COORD_VERTEX Q(	sharpness / (2.5f * freq.x),
							sharpness / (2.5f * freq.y),
							sharpness / (2.5f * freq.z),
							sharpness / (2.5f * freq.h));

		GL_COORD_VERTEX kNx(	xDir.x * A.x * freq.x,
								xDir.y * A.y * freq.y,
								xDir.z * A.z * freq.z,
								xDir.h * A.h * freq.h);
		GL_COORD_VERTEX kNy(	sharpness * 0.25,
								sharpness * 0.25,
								sharpness * 0.25,
								sharpness * 0.25);
		GL_COORD_VERTEX kNz(	zDir.x * A.x * freq.x,
								zDir.y * A.y * freq.y,
								zDir.z * A.z * freq.z,
								zDir.h * A.h * freq.h);

		GL_COORD_VERTEX kTx(	sharpness * 0.25 * xDir.x * zDir.x,
								sharpness * 0.25 * xDir.y * zDir.y,
								sharpness * 0.25 * xDir.z * zDir.z,
								sharpness * 0.25 * xDir.h * zDir.h);
		GL_COORD_VERTEX kTy(	- kNz.x,
								- kNz.y,
								- kNz.z,
								- kNz.h);
		GL_COORD_VERTEX kTz(	sharpness * 0.25 * zDir.x * zDir.x,
								sharpness * 0.25 * zDir.y * zDir.y,
								sharpness * 0.25 * zDir.z * zDir.z,
								sharpness * 0.25 * zDir.h * zDir.h);

		GL_COORD_VERTEX kBx(	sharpness * 0.25 * xDir.x * xDir.x,
								sharpness * 0.25 * xDir.y * xDir.y,
								sharpness * 0.25 * xDir.z * xDir.z,
								sharpness * 0.25 * xDir.h * xDir.h);
		GL_COORD_VERTEX kBy(	- kNx.x,
								- kNx.y,
								- kNx.z,
								- kNx.h);
		GL_COORD_VERTEX kBz(	sharpness * 0.25 * xDir.x * zDir.x,
								sharpness * 0.25 * xDir.y * zDir.y,
								sharpness * 0.25 * xDir.z * zDir.z,
								sharpness * 0.25 * xDir.h * zDir.h);

		params.addParameter("",xDir);
		params.addParameter("",zDir);
		params.addParameter("",A);
		params.addParameter("",freq);
		params.addParameter("",phase);
		params.addParameter("",Q);
		params.addParameter("",kNx);
		params.addParameter("",kNy);
		params.addParameter("",kNz);
		params.addParameter("",kTx);
		params.addParameter("",kTy);
		params.addParameter("",kTz);
		params.addParameter("",kBx);
		params.addParameter("",kBy);
		params.addParameter("",kBz);
		params.addParameter("",LPOS);

		CVertexShader *vs = m_pShader->glGetVertexShader();
		vs->setProgramParameters(params);

		params2.addParameter("",GL_COORD_VERTEX(0.0f,0.6f,0.8f,0.8f));
		params2.addParameter("",GL_COORD_VERTEX(8.0f,0.0f,0.0f,1.0f));

		CFragmentShader *fp = m_pShader->glGetFragmentShader();
		fp->setProgramParameters(params2);
	}

private:
	CShader					*m_pShader;
	CShaderProgram::CProgramParameters	params;
	CShaderProgram::CProgramParameters	params2;
};



class SeeGround : public CSimpleObject
{
public:
	SeeGround(	float width,float height,float scale,
				CTextureObject *map)
	:m_width(width),m_height(height),m_scale(scale)
	{
		CTextureUnitSetup setup;
		setup.setDiffuseMap(map);
		tmuSetup = setup.glBuildSetup();
	};
	virtual ~SeeGround() {};

	virtual void glRender()
	{
		CRaptorDisplay::glRender(tmuSetup);
		glColor4f(0.5f,0.5f,0.0f,1.0f);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f,m_scale);
			glVertex3f(-m_scale * m_width, 0, -m_scale * m_height);

			glTexCoord2f(0.0f,0.0f);
			glVertex3f(-m_scale * m_width, 0, m_scale * m_height);

			glTexCoord2f(m_scale,0.0f);
			glVertex3f(m_scale * m_width, 0, m_scale * m_height);

			glTexCoord2f(m_scale,m_scale);
			glVertex3f(m_scale * m_width, 0, -m_scale * m_height);
		glEnd();
	}

private:
	float m_width;
	float m_height;
	float m_scale;
	RAPTOR_HANDLE	tmuSetup;
};

class CTextureWaves : public CSimpleObject
{
public:
	CTextureWaves()
	{
		CRaptorDisplayConfig attrs;
		attrs.x = 0;
		attrs.y = 0;
		attrs.width = TABLE_SIZE;
		attrs.height = TABLE_SIZE;
		attrs.caption = "texture waves";
		attrs.refresh_rate;
		attrs.display_mode = CGL_RGBA | CGL_HARDWARE | CGL_RENDER_TEXTURE;

		pBuffer = Raptor::glCreateDisplay(attrs);
		RAPTOR_HANDLE handle;
		pBuffer->glBindDisplay(handle);
			
			CRenderingProperties *rp = pBuffer->getRenderingProperties();
			rp->setTexturing(CRenderingProperties::ENABLE);
			rp->setDepthTest(CRenderingProperties::DISABLE);
			rp->setCullFace(CRenderingProperties::DISABLE);
			rp->setLighting(CRenderingProperties::DISABLE);
			rp->clear(CGL_RGBA|CGL_DEPTH);
			CViewPoint *vpoint = pBuffer->getViewPoint();
			vpoint->setViewVolume(-1.0,1.0,-1.0,1.0,-1.0,1.0,CViewPoint::ORTHOGRAPHIC);
			vpoint->glRenderViewPointModel();
			
			pShader = new CShader("WATER_SHADER2");
			CVertexShader *vp = pShader->glGetVertexShader();
			vp->glLoadProgram(waterShader4.data());
			vp->glStop();
			CFragmentShader *fp = pShader->glGetFragmentShader();
			fp->glLoadProgram(waterFragments2.data());
			fp->glStop();
		pBuffer->glUnBindDisplay();

		CTextureFactory &factory = CTextureFactory::getDefaultFactory();
		pMap = factory.glCreateDynamicTexture(	CTextureObject::CGL_COLOR24_ALPHA,
												CTextureObject::CGL_OPAQUE,
												CTextureObject::CGL_BILINEAR,
												pBuffer);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

		pCosTable = factory.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_OPAQUE,CTextureObject::CGL_BILINEAR);
        pCosTable->setSize(TABLE_SIZE,1);
		pCosTable->glSetTransparency(255);
        pCosTable->allocateTexels();
		unsigned char *cosTable = pCosTable->getTexels();
        int i=0;
		for (i=0;i<4*TABLE_SIZE;i+=4)
		{
			cosTable[i] = 0;
			cosTable[i+1] = 255.999 * (0.5f * cos(2.0*PI*i/(4.0f * TABLE_SIZE)) + 0.5f)
							* pow((0.5f * sin(2.0*PI*i/(4.0f * TABLE_SIZE)) + 0.5f),1);
			cosTable[i+2] = 0;
			cosTable[i+3] = 255;
		}
		factory.glLoadTexture(pCosTable,".BUFFER");

		srand((unsigned)time( NULL ));
		float baseAngle = (float)(PI / 2.0f);

		for (i=0;i<8;i++)
		{
			speeds[i] = 0.5f + ((float)(rand() % 128)) / 256.0f;

			int n = (rand() % 32) - 16;
			if ((n == 0) || ( n == 1) || ( n == 2))
				n = 3;
			else if ((n == -1) || (n == -2))
				n = -3;

			float angle = atan(1.0 / n);
			freqs[i] = fabs((float)n) / cos(angle);
			angles[i] = angle + baseAngle;
		}

        props.clear(0);
        props.setTexturing(CRenderingProperties::ENABLE);
        props.setLighting(CRenderingProperties::DISABLE);
	};

	virtual ~CTextureWaves() 
    {
        Raptor::glDestroyDisplay(pBuffer);
    };

	virtual void glRender(bool bShow)
	{
		RAPTOR_HANDLE handle;
		pBuffer->glBindDisplay(handle);
			CVertexShader *vp = pShader->glGetVertexShader();
			float t = CTimeObject::GetGlobalTime();
			GL_COORD_VERTEX v(	0.2 * t,
								0.1 * t,
								0.5 * t,
								0);
			vp->glRender();
			vp->glProgramParameter(0,v);
			for (int i=0;i<8;i++)
			{
				float angle = angles[i];
				GL_COORD_VERTEX dir( cos(angle), sin(angle), sin(angle), -cos(angle) );
				vp->glProgramParameter(i+1,dir);
			}
			GL_COORD_VERTEX w123(freqs[0],freqs[1],freqs[2],1.0f);
			vp->glProgramParameter(9,w123);
			GL_COORD_VERTEX w456(freqs[3],freqs[4],freqs[5],1.0f);
			vp->glProgramParameter(10,w456);
			GL_COORD_VERTEX w789(freqs[6],freqs[7],freqs[0],1.0f);
			vp->glProgramParameter(11,w789);

			CFragmentShader *fp = pShader->glGetFragmentShader();
			fp->glRender();
			pCosTable->glRender();
			glBegin(GL_QUADS);
				glTexCoord4f(0.0f,0.0f,0.0f,0.0f);glVertex3f(-1.0f,-1.0f,0.0f);
				glTexCoord4f(2.0f,0.0f,0.0f,0.0f);glVertex3f(1.0f,-1.0f,0.0f);
				glTexCoord4f(2.0f,1.0f,0.0f,0.0f);glVertex3f(1.0f,1.0f,0.0f);
				glTexCoord4f(0.0f,1.0f,0.0f,0.0f);glVertex3f(-1.0f,1.0f,0.0f);
			glEnd();
			vp->glStop();
			fp->glStop();
		pBuffer->glUnBindDisplay();

		pMap->glRender();

		if (bShow)
		{
            props.glPushProperties();

			glPushMatrix();
				glTranslatef(0.0f,0.0f,-5.0f);
				glBegin(GL_QUADS);
					glTexCoord2f(0.0f,0.0f);glVertex3f(-2.0f,-2.0f,0.0f);
					glTexCoord2f(1.0f,0.0f);glVertex3f(2.0f,-2.0f,0.0f);
					glTexCoord2f(1.0f,1.0f);glVertex3f(2.0f,2.0f,0.0f);
					glTexCoord2f(0.0f,1.0f);glVertex3f(-2.0f,2.0f,0.0f);
				glEnd();
			glPopMatrix();

            props.glPopProperties();
		}
	}

	CTextureObject* GetNormalMap(void) const
	{ return pMap; };

private:
    CRenderingProperties props;
	CTextureObject	*pMap;
	CTextureObject	*pCosTable;
	CRaptorDisplay	*pBuffer;
	CShader			*pShader;
	float			angles[8];
	float			freqs[8];
	float			speeds[8];
};


class CSky : public CShadedGeometry
{
public:
	CSky()
	{
		CShader *sh = getShader();
		CTextureUnitSetup* tus = sh->glGetTextureUnitsSetup();

		CTextureFactory &factory = CTextureFactory::getDefaultFactory();
  		CTextureObject *T = factory.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,
													CTextureObject::CGL_OPAQUE,
													CTextureObject::CGL_BILINEAR);

		T->glSetTransparency(255);

		CTextureFactoryConfig& config = factory.getConfig();
		const CTextureFactoryConfig::ICompressor *compressor = config.getCurrentCompressor();
		if (0 < config.getNumCompressors())
			config.setCurrentCompressor(config.getCompressor("OpenGL"));

		factory.glLoadTexture(T,"Datas\\ciel_07.jpg");
		tus->setDiffuseMap(T);
	}

	virtual ~CSky() {};

	CSky& operator=(const CGeometry &g)
	{
		CShadedGeometry::operator=(g);
		return *this;
	}
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CVertexShadersDisplay::CVertexShadersDisplay():
	see(NULL),showBump(false),
	shaderModifier(NULL),pLight(NULL),
	ground(NULL),tw(NULL)
{
}

CVertexShadersDisplay::~CVertexShadersDisplay()
{
}


void CVertexShadersDisplay::UnInit()
{
}

void CVertexShadersDisplay::ReInit()
{
	CGenericDisplay::ReInit();

	if (shaderModifier)
		shaderModifier->animate(true);

    CRaptorDisplay* const pDisplay = CRaptorDisplay::GetCurrentDisplay();
    CRenderingProperties *rp = pDisplay->getRenderingProperties();
    rp->setTexturing(CRenderingProperties::ENABLE);
}

void CVertexShadersDisplay::Init()
{
	CGenericDisplay::Init();

	CTextureFactory &factory = CTextureFactory::getDefaultFactory();

	See *water = new See(3000.0f,3000.0f,100,100);
    CGeometry::CRenderingModel l_model(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
    l_model.addModel(CGeometry::CRenderingModel::CGL_TEXTURE);
	water->setRenderingModel(l_model);

	CShader *pShader = water->getShader();
	see = water;

	CTextureUnitSetup *ts = pShader->glGetTextureUnitsSetup();
	CTextureObject* T = factory.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,
												CTextureObject::CGL_ALPHA_TRANSPARENT,
												CTextureObject::CGL_BILINEAR);

	T->glSetTransparency(128);
	factory.glLoadTexture(T,"Datas\\water006.jpg");
	ts->setDiffuseMap(T);

	CPersistence *p = CPersistence::FindObject("main_textures");
	CTextureSet	*tf = (CTextureSet*)p;
	T = tf->getTexture("Datas\\ciel_07_small.jpg");
	ts->setEnvironmentMap(T);

	CVertexShader *vp = pShader->glGetVertexShader();
    string fullWaterShader = waterShader+waterShader2+waterShader3;
	vp->glLoadProgram(fullWaterShader.data());
	vp->glStop();
	CFragmentShader *fp = pShader->glGetFragmentShader();
	fp->glLoadProgram(waterFragments.data());
	fp->glStop();

	shaderModifier = new ShaderModifier(pShader);

	T = factory.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_OPAQUE, CTextureObject::CGL_BILINEAR);
	T->glSetTransparency(255);
	factory.glLoadTexture(T,"Datas\\oldwood.jpg");

	ground = new SeeGround(200.0f,200.0f,10.0f,T);

	tw = new CTextureWaves();
	ts->setNormalMap(tw->GetNormalMap());


	sky = new CSky();
	CGeometry* skydome = (CGeometry*)CPersistence::FindObject("SKYDOME");
	*sky = *skydome;
    CGeometry::CRenderingModel l_model2(CGeometry::CRenderingModel::CGL_BACK_GEOMETRY);
    l_model2.addModel(CGeometry::CRenderingModel::CGL_TEXTURE);
	sky->setRenderingModel(l_model2);

	pScene = new C3DScene("SHADER SCENE");
}



void CVertexShadersDisplay::Display()
{
	if (reinit)
		ReInit();

	float dt = CTimeObject::GetGlobalTime();

	tw->glRender(showBump);

	glPushMatrix();

	glTranslatef(0.0f,0.0f,-1500.0f);
	glRotatef(5.0f,1.0f,0.0f,0.0f);
	glRotatef(dt*360.0,0.0f,1.0f,0.0f);

	glColor4f(1.0f,1.0f,1.0f,1.0f);

	GL_COORD_VERTEX lpos(1000.0f*cos(3000*dt*2*PI/180),500.0f,1000.0f*sin(3000*dt*2*PI/180),1.0f);
	LPOS = lpos;

	glPushMatrix();
		glTranslatef(0.0f,-200.0f,0.0f);
		ground->glRender();
	glPopMatrix();


	sky->glRender();

    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_BLEND);
	see->glRender();
    glPopAttrib();

	glPopMatrix();
}