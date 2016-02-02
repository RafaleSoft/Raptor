// PhongShader.cpp: implementation of the CPhongShader class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_EMBMSHADER_H__99A5AF45_D5C7_4F43_851C_A31FC52DB237__INCLUDED_)
	#include "EMBMShader.h"
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
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif


RAPTOR_NAMESPACE

int CEMBMShader::environmentMap = -1;


CEMBMShader::CEMBMShader(void)
	:CBumpShader("EMBM_SHADER"),
	m_bEnabled(false)
{
}

CEMBMShader::CEMBMShader(const CEMBMShader& shader)
	:CBumpShader(shader),
	m_bEnabled(shader.m_bEnabled)
{
}

CShader* CEMBMShader::glClone(const std::string& newShaderName) const
{
	CEMBMShader* embm = new CEMBMShader(*this);
	if (!newShaderName.empty())
		embm->setName(newShaderName);
	embm->m_bEnabled = m_bEnabled;
	return embm;
}

CEMBMShader::~CEMBMShader(void)
{
}

void CEMBMShader::glInit(const std::string &bump_vertexshader,
						 const std::string &bump_pixelshader)
{
	//CVertexProgram *vp = glGetVertexProgram("PPIXEL_BUMP_VTX_PROGRAM");
	//!	First create the program, to get it with the shader
	//!	and unset auto delete shader
	CVertexProgram *vp = new CVertexProgram("PPIXEL_EMBM_VTX_PROGRAM");
	std::string embm_vertexshader = bump_vertexshader;
	size_t pos = embm_vertexshader.find("#version");
	if (pos < embm_vertexshader.length())
	{
		size_t pos2 = embm_vertexshader.find("\n",pos);
		embm_vertexshader.insert(pos2+1,string("#define EMBM_RENDERING 1\n\n"));
	}
	vp->glLoadProgram(embm_vertexshader);
	vp->glStop();

	vp = glGetVertexProgram("PPIXEL_EMBM_VTX_PROGRAM");
	CShaderProgram::CProgramParameters params;
	params.addParameter("tangent",CShaderProgram::ADDITIONAL_PARAM1);
	vp->setProgramParameters(params);

	//!	First create the program, to get it with the shader
	//!	and unset auto delete shader
	CFragmentProgram *fp = new CFragmentProgram("PPIXEL_EMBM_TEX_PROGRAM");
	std::string embm_pixelshader = bump_pixelshader;
	pos = embm_pixelshader.find("#version");
	if (pos < embm_pixelshader.length())
	{
		size_t pos2 = embm_pixelshader.find("\n",pos);
		embm_pixelshader.insert(pos2+1,string("#define EMBM_RENDERING 1\n\n"));
	}
	fp->glLoadProgram(embm_pixelshader);
	fp->glStop();

	fp = glGetFragmentProgram("PPIXEL_EMBM_TEX_PROGRAM");
	CShaderProgram::CProgramParameters params2;
	fp->setProgramParameters(params2);

	glCompileShader();
}

void CEMBMShader::glRender(void)
{
	CBumpShader::glRender();

#if defined(GL_ARB_shader_objects)
	if (m_bEnabled)
	{
		const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

		if (environmentMap < 0)
		{
			GLhandleARB program = pExtensions->glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
			environmentMap = pExtensions->glGetUniformLocationARB(program, "environmentMap");
		}

		if (environmentMap >= 0)
			pExtensions->glUniform1iARB(environmentMap,CTextureUnitSetup::IMAGE_UNIT_3);
	}
#endif
}
