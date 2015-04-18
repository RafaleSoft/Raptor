#version 120

varying vec4 vertex;
varying vec4 normal;

uniform mat4 vertexMat;
uniform mat4 normalMat;

void main (void)
{
	gl_Position = gl_MultiTexCoord2;
	/*gl_TexCoord[0] = gl_MultiTexCoord0;*/
	vertex = vertexMat * gl_Vertex;
	normal = normalMat * vec4(gl_Normal,1);
}
