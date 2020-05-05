#include "StdAfx.h"
#include <sstream>


extern const std::string vp2_src =
"#version 460 compatibility\n\
\n\
layout(location = 0) in vec4 i_Position; \n\
layout(location = 3) in vec4 i_Color; \n\
layout(location = 6) in vec4 i_Min; \n\
layout(location = 7) in vec4 i_Max; \n\
\n\
out vec4 v_color; \n\
out vec4 v_Min; \n\
out vec4 v_Max; \n\
\n\
void main (void) \n\
{\n\
	v_color = i_Color; \n\
\n\
	v_Min = i_Position + vec4(i_Min.x, i_Min.y, i_Min.z, 0.0); \n\
	v_Max = i_Position + vec4(i_Max.x, i_Max.y, i_Max.z, 0.0); \n\
}\n\
";

extern const std::string gp2_src =
"#version 460 compatibility\n\
\n\
//	Expect the geometry shader extension to be available, warn if not. \n\
#extension GL_ARB_geometry_shader4 : enable \n\
\n\
in vec4 v_color[]; \n\
in vec4 v_Min[]; \n\
in vec4 v_Max[]; \n\
\n\
layout(points) in; \n\
layout(triangle_strip, max_vertices=18) out; \n\
\n\
layout(location = 1) out vec4 g_TexCoord; \n\
out vec4 g_color; \n\
\n\
void main() \n\
{\n\
	g_color = v_color[0]; \n\
\n\
	// top: Back right \n\
	vec4 pos = vec4(v_Max[0].x, v_Max[0].y, v_Min[0].z, 1.0); \n\
	gl_Position = gl_ModelViewProjectionMatrix * pos; \n\
	g_TexCoord = vec4(0.0,1.0,0.0,0.0); \n\
	EmitVertex(); \n\
	// top: Back Left \n\
	pos = vec4(v_Min[0].x, v_Max[0].y, v_Min[0].z, 1.0); \n\
	gl_Position = gl_ModelViewProjectionMatrix * pos; \n\
	g_TexCoord = vec4(0.0,0.0,0.0,0.0); \n\
	EmitVertex(); \n\
	// top: Front Right \n\
	pos = vec4(v_Max[0].x, v_Max[0].y, v_Max[0].z, 1.0); \n\
	gl_Position = gl_ModelViewProjectionMatrix * pos; \n\
	g_TexCoord = vec4(1.0,1.0,0.0,0.0); \n\
	EmitVertex(); \n\
	// top: Front Left \n\
	pos = vec4(v_Min[0].x, v_Max[0].y, v_Max[0].z, 1.0); \n\
	gl_Position = gl_ModelViewProjectionMatrix * pos; \n\
	g_TexCoord = vec4(1.0,0.0,0.0,0.0); \n\
	EmitVertex(); \n\
\n\
	// front: Top Left \n\
	pos = vec4(v_Min[0].x, v_Max[0].y, v_Max[0].z, 1.0); \n\
	gl_Position = gl_ModelViewProjectionMatrix * pos; \n\
	g_TexCoord = vec4(0.0,1.0,0.0,0.0); \n\
	EmitVertex(); \n\
	// front: Bottom Left \n\
	pos = vec4(v_Min[0].x, v_Min[0].y, v_Max[0].z, 1.0); \n\
	gl_Position = gl_ModelViewProjectionMatrix * pos; \n\
	g_TexCoord = vec4(0.0,0.0,0.0,0.0); \n\
	EmitVertex(); \n\
	// front: Top Right \n\
	pos = vec4(v_Max[0].x, v_Max[0].y, v_Max[0].z, 1.0); \n\
	gl_Position = gl_ModelViewProjectionMatrix * pos; \n\
	g_TexCoord = vec4(1.0,1.0,0.0,0.0); \n\
	EmitVertex(); \n\
	// front: Bottom Right \n\
	pos = vec4(v_Max[0].x, v_Min[0].y, v_Max[0].z, 1.0); \n\
	gl_Position = gl_ModelViewProjectionMatrix * pos; \n\
	g_TexCoord = vec4(1.0,0.0,0.0,0.0); \n\
	EmitVertex(); \n\
\n\
	// right: Top Right \n\
	pos = vec4(v_Max[0].x, v_Max[0].y, v_Min[0].z, 1.0); \n\
	gl_Position = gl_ModelViewProjectionMatrix * pos; \n\
	g_TexCoord = vec4(0.0,1.0,0.0,0.0); \n\
	EmitVertex(); \n\
	// right: Bottom Right \n\
	pos = vec4(v_Max[0].x, v_Min[0].y, v_Min[0].z, 1.0); \n\
	gl_Position = gl_ModelViewProjectionMatrix * pos; \n\
	g_TexCoord = vec4(0.0,0.0,0.0,0.0); \n\
	EmitVertex(); \n\
\n\
	// back: Top Left \n\
	pos = vec4(v_Min[0].x, v_Max[0].y, v_Min[0].z, 1.0); \n\
	gl_Position = gl_ModelViewProjectionMatrix * pos; \n\
	g_TexCoord = vec4(1.0,1.0,0.0,0.0); \n\
	EmitVertex(); \n\
	// back: Bottom Left \n\
	pos = vec4(v_Min[0].x, v_Min[0].y, v_Min[0].z, 1.0); \n\
	gl_Position = gl_ModelViewProjectionMatrix * pos; \n\
	g_TexCoord = vec4(1.0,0.0,0.0,0.0); \n\
	EmitVertex(); \n\
\n\
	// left: Top Left \n\
	pos = vec4(v_Min[0].x, v_Max[0].y, v_Max[0].z, 1.0); \n\
	gl_Position = gl_ModelViewProjectionMatrix * pos; \n\
	g_TexCoord = vec4(0.0,1.0,0.0,0.0); \n\
	EmitVertex(); \n\
	// left: Bottom Left \n\
	pos = vec4(v_Min[0].x, v_Min[0].y, v_Max[0].z, 1.0); \n\
	gl_Position = gl_ModelViewProjectionMatrix * pos; \n\
	g_TexCoord = vec4(0.0,0.0,0.0,0.0); \n\
	EmitVertex(); \n\
\n\
	// bottom: Front Left\n\
	pos = vec4(v_Min[0].x, v_Min[0].y, v_Max[0].z, 1.0); \n\
	gl_Position = gl_ModelViewProjectionMatrix * pos; \n\
	g_TexCoord = vec4(0.0,1.0,0.0,0.0); \n\
	EmitVertex(); \n\
	// bottom: Back Left\n\
	pos = vec4(v_Min[0].x, v_Min[0].y, v_Min[0].z, 1.0); \n\
	gl_Position = gl_ModelViewProjectionMatrix * pos; \n\
	g_TexCoord = vec4(0.0,0.0,0.0,0.0); \n\
	EmitVertex(); \n\
	// bottom: Front Right\n\
	pos = vec4(v_Max[0].x, v_Min[0].y, v_Max[0].z, 1.0); \n\
	gl_Position = gl_ModelViewProjectionMatrix * pos; \n\
	g_TexCoord = vec4(1.0,1.0,0.0,0.0); \n\
	EmitVertex(); \n\
	// bottom: Back Right\n\
	pos = vec4(v_Max[0].x, v_Min[0].y, v_Min[0].z, 1.0); \n\
	gl_Position = gl_ModelViewProjectionMatrix * pos; \n\
	g_TexCoord = vec4(1.0,0.0,0.0,0.0); \n\
	EmitVertex(); \n\
\n\
	EndPrimitive(); \n\
}\n\
";


extern const std::string fp2_src =
"#version 460\n\
\n\
uniform	sampler2D diffuseMap; \n\
\n\
in vec4 g_color; \n\
layout(location = 1) in vec4 g_TexCoord; \n\
\n\
layout(location = 0) out vec4 o_Color;	\n\
\n\
void main (void) \n\
{\n\
	o_Color = g_color * texture2D(diffuseMap,vec2(g_TexCoord.st)); \n\
}\n\
";


extern const std::string vp_src =
"#version 460 compatibility\n\
\n\
layout(location = 0) in vec4 i_Position; \n\
layout(location = 1) in float i_Size; \n\
layout(location = 3) in vec4 i_Color; \n\
layout(location = 5) in float i_Angle; \n\
\n\
out float angle; \n\
out float size; \n\
out vec4 v_color; \n\
\n\
void main (void) \n\
{\n\
	vec4 pos = vec4(vec3(i_Position.xyz),1.0); \n\
	gl_Position =  gl_ModelViewProjectionMatrix * pos; \n\
	angle = i_Angle; \n\
	size = i_Size; \n\
	v_color = i_Color; \n\
}\n\
";

extern const std::string gp_src =
"#version 460\n\
\n\
//	Expect the geometry shader extension to be available, warn if not. \n\
#extension GL_ARB_geometry_shader4 : enable \n\
\n\
in float angle[]; \n\
in float size[]; \n\
in vec4 v_color[]; \n\
\n\
layout(points) in; \n\
layout(triangle_strip, max_vertices=4) out; \n\
layout(location = 1) out vec4 g_TexCoord; \n\
out vec4 g_color; \n\
\n\
void main() \n\
{\n\
	float cs = cos(angle[0]); \n\
	float ss = sin(angle[0]);	\n\
	float Hx = 0.5 * size[0] * (cs - ss);	\n\
	float Hy = 0.5 * size[0] * (cs + ss);	\n\
	g_color = v_color[0]; \n\
	\n\
	gl_Position = gl_in[0].gl_Position + vec4(-Hx, -Hy, 0.0, 0.0); \n\
	g_TexCoord = vec4(0.0,0.0,0.0,0.0); \n\
	EmitVertex(); \n\
\n\
	gl_Position = gl_in[0].gl_Position + vec4(Hy,-Hx,0.0,0.0); \n\
	g_TexCoord = vec4(1.0,0.0,0.0,0.0); \n\
	EmitVertex(); \n\
\n\
	gl_Position = gl_in[0].gl_Position + vec4(-Hy,Hx,0.0,0.0); \n\
	g_TexCoord = vec4(0.0,1.0,0.0,0.0); \n\
	EmitVertex(); \n\
\n\
	gl_Position = gl_in[0].gl_Position + vec4(Hx,Hy,0.0,0.0); \n\
	g_TexCoord = vec4(1.0,1.0,0.0,0.0); \n\
	EmitVertex(); \n\
\n\
	EndPrimitive(); \n\
}\n\
";

extern const std::string fp_src =
"#version 460\n\
\n\
uniform	sampler2D diffuseMap; \n\
\n\
in vec4 g_color; \n\
layout(location = 1) in vec4 g_TexCoord; \n\
layout(location = 0) out vec4 o_Color;	\n\
\n\
void main (void) \n\
{\n\
	o_Color = g_color * texture2D(diffuseMap,vec2(g_TexCoord.st)); \n\
}\n\
";
