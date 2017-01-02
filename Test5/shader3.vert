#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec4 i_Position;
layout(location = 1) in vec4 i_Color;

out gl_PerVertex
{
	vec4 gl_Position;
};

layout(location = 0) out vec4 v_Color;

void main() {
	gl_Position = i_Position;
	v_Color = i_Color;
} 
