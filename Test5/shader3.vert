#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 proj;
} ubo;

layout(location = 0) in vec4 i_Position;
layout(location = 1) in vec2 i_TexCoord;
layout(location = 2) in vec4 i_Color;

out gl_PerVertex
{
	vec4 gl_Position;
};

layout(location = 0) out vec4 v_Color;
layout(location = 1) out vec2 v_TexCoord;

void main() {
	vec4 pos = ubo.proj * ubo.model * i_Position;
	gl_Position = pos / pos.w;
	//gl_Position = ubo.model * i_Position;
	v_Color = i_Color;
	v_TexCoord = i_TexCoord;
}
