// PhongShader.cpp: implementation of the CPhongShader class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_BUMPSHADER_H__6201C4A1_1F09_41C4_836F_2AAC79D36A42__INCLUDED_)
	#include "BumpShader.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
#endif
#if !defined(AFX_TEXTUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "GLHierarchy/TextureUnitSetup.h"
#endif
#if !defined(AFX_VERTEXPROGRAM_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
    #include "GLHierarchy/VertexProgram.h"
#endif
#if !defined(AFX_FRAGMENTPROGRAM_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
    #include "GLHierarchy/FragmentProgram.h"
#endif
#if !defined(AFX_LIGHTATTRIBUTES_H__B0A3AF95_90DC_4185_9747_B7F631DDB2BF__INCLUDED_)
	#include "LightAttributes.h"
#endif
#if !defined(AFX_3DENGINEMATRIX_H__6CD1110E_1174_4f38_A452_30FB312022D0__INCLUDED_)
	#include "Engine/3DEngineMatrix.h"
#endif


RAPTOR_NAMESPACE

int CBumpShader::lightEnable = -1;
int CBumpShader::diffuseMap = -1;
int CBumpShader::normalMap = -1;
int CBumpShader::eyePos = -1;

CBumpShader::CBumpShader(void)
	:CShader("BUMP_SHADER")
{
}

CBumpShader::~CBumpShader(void)
{
}

void CBumpShader::glInit(void)
{
	CVertexProgram *vp = glGetVertexProgram("PPIXEL_BUMP_VTX_PROGRAM");
	CShaderProgram::CProgramParameters params;
	params.addParameter("tangent",CShaderProgram::ADDITIONAL_PARAM1);
	vp->setProgramParameters(params);

	CFragmentProgram *fp = glGetFragmentProgram("PPIXEL_BUMP_TEX_PROGRAM");
	CShaderProgram::CProgramParameters params2;
	fp->setProgramParameters(params2);

	glCompileShader();
}

void CBumpShader::glStop(void)
{
	CShader::glStop();
}

void CBumpShader::glRender(void)
{
#if defined(GL_ARB_shader_objects)
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

	bool *pGLLights = CLightAttributes::getActiveGLLights();
	int bLights[CLightAttributes::MAX_LIGHTS] = {0,0,0,0,0,0,0,0};

	for (unsigned int i=0;i<CLightAttributes::MAX_LIGHTS;i++)
		bLights[i] = (pGLLights[i] ? 1 : 0);

	CShader::glRender();
	GLhandleARB program = pExtensions->glGetHandleARB(GL_PROGRAM_OBJECT_ARB);

	if (lightEnable < 0)
		lightEnable = pExtensions->glGetUniformLocationARB(program,"lightEnable");
	pExtensions->glUniform1ivARB(lightEnable,CLightAttributes::MAX_LIGHTS,bLights);

	if (diffuseMap < 0)
		diffuseMap = pExtensions->glGetUniformLocationARB(program,"diffuseMap");
	pExtensions->glUniform1iARB(diffuseMap,CTextureUnitSetup::IMAGE_UNIT_0);

	if (normalMap < 0)
		normalMap = pExtensions->glGetUniformLocationARB(program,"normalMap");
	pExtensions->glUniform1iARB(normalMap,CTextureUnitSetup::IMAGE_UNIT_1);

	C3DEngineMatrix T;
	glGetTransposeFloatv(GL_MODELVIEW_MATRIX,T);
	GL_COORD_VERTEX V;
	V.x = -(T[0]*T[3] + T[4]*T[7] + T[8]*T[11]);
	V.y = -(T[1]*T[3] + T[5]*T[7] + T[9]*T[11]);
	V.z = -(T[2]*T[3] + T[6]*T[7] + T[10]*T[11]);
	V.h = 1.0f;

	if (eyePos < 0)
		eyePos = pExtensions->glGetUniformLocationARB(program,"eyePos");
	pExtensions->glUniform4fvARB(eyePos,1,V);
#else
	CShader::glRender();
#endif
}
