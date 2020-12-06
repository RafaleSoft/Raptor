// Object3DShadowAttributes.cpp: implementation of the CObject3DShadowAttributes class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_OBJECT3DSHADOWATTRIBUTES_H__4FAFE331_A695_471C_AD48_0F541948AAAD__INCLUDED_)
    #include "Subsys/Object3DShadowAttributes.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif


const std::string shadow_vshader =
"#version 440 compatibility \n\
\n\
layout(location = 0) in vec4 i_Position; \n\
\n\
void main(void) \n\
{\n\
	gl_Position = gl_ModelViewProjectionMatrix * i_Position; \n\
}";
const std::string shadow_fshader =
"#version 440 \n\
\n\
layout(location = 0) out vec4 o_Color; \n\
\n\
void main(void) \n\
{ \n\
	o_Color = vec4(1.0,1.0,1.0,1.0); \n\
}";

const std::string shadow_vshader2 =
"#version 440\n\
\n\
layout(location = 0) in vec4 i_Position; \n\
\n\
void main(void) \n\
{\n\
	gl_Position = i_Position; \n\
}";
const std::string shadow_gshader =
"#version 440 compatibility\n\
#extension GL_ARB_geometry_shader4 : enable \n\
layout(lines) in; \n\
layout(triangle_strip, max_vertices = 4) out; \n\
\n\
uniform vec4 lpos; \n\
uniform float extrusion; \n\
\n\
void main(void) \n\
{ \n\
	vec4 p1 = gl_in[0].gl_Position; \n\
	vec4 p2 = gl_in[1].gl_Position; \n\
\n\
	vec4 lvect = vec4(p1.x - lpos.x, p1.y - lpos.y, p1.z - lpos.z, 1.0); \n\
	vec4 pos = extrusion * normalize(lvect) + p1; \n\
\n\
	gl_Position = gl_ModelViewProjectionMatrix * p1; \n\
	EmitVertex();\n\
	gl_Position = gl_ModelViewProjectionMatrix * pos; \n\
	EmitVertex();\n\
\n\
	lvect = vec4(p2.x - lpos.x, p2.y - lpos.y, p2.z - lpos.z, 1.0); \n\
	pos = extrusion * normalize(lvect) + p2; \n\
\n\
	gl_Position = gl_ModelViewProjectionMatrix * p2; \n\
	EmitVertex();\n\
	gl_Position = gl_ModelViewProjectionMatrix * pos; \n\
	EmitVertex();\n\
\n\
	EndPrimitive(); \n\
}";


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObject3DShadowAttributes::CObject3DShadowAttributes()
{
    visibilityCount = 0;
    visibilityQuery = 0;
    queryIssued = false;

	reBuild = false;
	renderOcclusion = false;

    m_pLight = NULL;
    m_pObject = NULL;

	pStage = NULL;
	pStage2 = NULL;

    const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

#ifdef GL_ARB_imaging
	glBlendColorARB = pExtensions->glBlendColorARB;
#endif
#ifdef GL_EXT_stencil_two_side
	glActiveStencilFaceEXT = pExtensions->glActiveStencilFaceEXT;
#endif

#if defined(GL_ARB_occlusion_query)
	if (pExtensions->glGenQueriesARB != NULL)
		pExtensions->glGenQueriesARB(1,&visibilityQuery);

	CATCH_GL_ERROR
#endif
}

CObject3DShadowAttributes::~CObject3DShadowAttributes()
{

}

bool CObject3DShadowAttributes::glBuildShaders()
{
	if ((NULL != pStage) && (NULL != pStage2))
		return false;

	pStage = new COpenGLShaderStage("SHADOW_SHADER");
	CVertexShader* vs = pStage->glGetVertexShader("SHADOW_VERTEX_SHADER");
	vs->glLoadProgram(shadow_vshader);
	CFragmentShader* fs = pStage->glGetFragmentShader("SHADOW_FRAGMANT_SHADER");
	fs->glLoadProgram(shadow_fshader);
	pStage->glCompileShader();

	pStage2 = new COpenGLShaderStage("SHADOW_SHADER2");
	vs = pStage2->glGetVertexShader("SHADOW_VERTEX_SHADER2");
	vs->glLoadProgram(shadow_vshader2);
	fs = pStage2->glGetFragmentShader("SHADOW_FRAGMANT_SHADER2");
	fs->glLoadProgram(shadow_fshader);
	CGeometryShader* gs = pStage2->glGetGeometryShader("SHADOW_GEOMETRY_SHADER2");
	gs->glLoadProgram(shadow_gshader);

	CProgramParameters params;
	params.addParameter<GL_COORD_VERTEX>("lpos", L);
	params.addParameter<float>("extrusion", z_max);
	pStage2->setProgramParameters(params);

	pStage2->glCompileShader();

	return true;
}
