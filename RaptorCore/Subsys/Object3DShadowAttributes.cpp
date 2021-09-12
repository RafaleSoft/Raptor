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
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
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
vec4 extrude(vec4 p1)\n\
{\n\
	vec4 lvect = p1 - lpos; \n\
	vec4 pos = extrusion * normalize(lvect) + p1; \n\
	return pos; \n\
}\n\
\n\
void main(void) \n\
{ \n\
	vec4 p1 = gl_in[0].gl_Position; \n\
	vec4 p2 = gl_in[1].gl_Position; \n\
\n\
	gl_Position = gl_ModelViewProjectionMatrix * extrude(p1); \n\
	EmitVertex();\n\
	gl_Position = gl_ModelViewProjectionMatrix * p1; \n\
	EmitVertex();\n\
\n\
	gl_Position = gl_ModelViewProjectionMatrix * extrude(p2); \n\
	EmitVertex();\n\
	gl_Position = gl_ModelViewProjectionMatrix * p2; \n\
	EmitVertex();\n\
\n\
	EndPrimitive(); \n\
}";

extern const std::string shadowbox_gshader =
"#version 440 compatibility \n\
\n\
//	Expect the geometry shader extension to be available, warn if not. \n\
#extension GL_ARB_geometry_shader4 : enable \n\
\n\
in vec4 v_color[]; \n\
uniform float extrusion; \n\
uniform vec4 lpos; \n\
\n\
layout(lines) in; \n\
layout(triangle_strip, max_vertices = 18) out; \n\
\n\
out vec4 g_color; \n\
\n\
vec4 extrude(vec4 p1)\n\
{\n\
	vec4 lvect = p1 - lpos; \n\
	vec4 pos = extrusion * normalize(lvect) + p1; \n\
	return pos; \n\
}\n\
\n\
void main()\n\
{\n\
	vec4 v_Min = gl_in[0].gl_Position;\n\
	vec4 v_Max = gl_in[1].gl_Position;\n\
\n\
	g_color = v_color[0];\n\
\n\
	// top: Back right\n\
	vec4 pos = vec4(v_Max.x, v_Max.y, v_Min.z, 1.0);\n\
	vec4 tbr = gl_ModelViewProjectionMatrix * extrude(pos);\n\
	gl_Position = tbr;\n\
	EmitVertex();\n\
	// top: Back Left\n\
	pos = vec4(v_Min.x, v_Max.y, v_Min.z, 1.0);\n\
	vec4 tbl = gl_ModelViewProjectionMatrix * extrude(pos);\n\
	gl_Position = tbl;\n\
	EmitVertex();\n\
	// top: Front Right\n\
	pos = vec4(v_Max.x, v_Max.y, v_Max.z, 1.0);\n\
	vec4 tfr = gl_ModelViewProjectionMatrix * extrude(pos);\n\
	gl_Position = tfr;\n\
	EmitVertex();\n\
	// top: Front Left \n\
	pos = vec4(v_Min.x, v_Max.y, v_Max.z, 1.0);\n\
	vec4 tfl = gl_ModelViewProjectionMatrix * extrude(pos);\n\
	gl_Position = tfl;\n\
	EmitVertex();\n\
\n\
	// front: Top Left\n\
	gl_Position = tfl;\n\
	EmitVertex();\n\
	// front: Bottom Left\n\
	pos = vec4(v_Min.x, v_Min.y, v_Max.z, 1.0);\n\
	vec4 bfl = gl_ModelViewProjectionMatrix * extrude(pos);\n\
	gl_Position = bfl;\n\
	EmitVertex();\n\
	// front: Top Right\n\
	gl_Position = tfr;\n\
	EmitVertex();\n\
	// front: Bottom Right\n\
	pos = vec4(v_Max.x, v_Min.y, v_Max.z, 1.0);\n\
	vec4 bfr = gl_ModelViewProjectionMatrix * extrude(pos);\n\
	gl_Position = bfr;\n\
	EmitVertex();\n\
\n\
	// right: Top back\n\
	gl_Position = tbr;\n\
	EmitVertex();\n\
	// right: Bottom back\n\
	pos = vec4(v_Max.x, v_Min.y, v_Min.z, 1.0);\n\
	vec4 bbr = gl_ModelViewProjectionMatrix * extrude(pos);\n\
	gl_Position = bbr;\n\
	EmitVertex();\n\
\n\
	// back: Top Left\n\
	gl_Position = tbl;\n\
	EmitVertex();\n\
	// back: Bottom Left\n\
	pos = vec4(v_Min.x, v_Min.y, v_Min.z, 1.0);\n\
	vec4 bbl = gl_ModelViewProjectionMatrix * extrude(pos);\n\
	gl_Position = bbl;\n\
	EmitVertex();\n\
\n\
	// left: Top front\n\
	gl_Position = tfl;\n\
	EmitVertex();\n\
	// left: Bottom front\n\
	gl_Position = bfl;\n\
	EmitVertex();\n\
\n\
	// bottom: Front Left\n\
	gl_Position = bfl;\n\
	EmitVertex();\n\
	// bottom: Back Left\n\
	gl_Position = bbl;\n\
	EmitVertex();\n\
	// bottom: Front Right\n\
	gl_Position = bfr;\n\
	EmitVertex();\n\
	// bottom: Back Right\n\
	gl_Position = bbr;\n\
	EmitVertex();\n\
\n\
	EndPrimitive();\n\
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
	pStageBox = NULL;

    const CRaptorGLExtensions *const pExtensions = IRaptor::glGetExtensions();

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
	if ((NULL != pStage) && (NULL != pStage2) && (NULL != pStageBox))
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
	params.addParameter<GL_COORD_VERTEX>("lpos", GL_COORD_VERTEX(0.0f,0.0f,0.0f,0.0f));
	params.addParameter<float>("extrusion", extrusion);
	pStage2->setProgramParameters(params);
	pStage2->glCompileShader();

	pStageBox = new COpenGLShaderStage("SHADOW_SHADER_BOX");
	vs = pStageBox->glGetVertexShader("SHADOW_VERTEX_SHADER_BOX");
	vs->glLoadProgram(shadow_vshader2);
	fs = pStageBox->glGetFragmentShader("SHADOW_FRAGMANT_SHADER_BOX");
	fs->glLoadProgram(shadow_fshader);
	gs = pStageBox->glGetGeometryShader("SHADOW_GEOMETRY_SHADER_BOX");
	gs->glLoadProgram(shadowbox_gshader);

	pStageBox->setProgramParameters(params);
	pStageBox->glCompileShader();

	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	pShaderBox = instance.m_pFilledBboxShader;

	return true;
}
