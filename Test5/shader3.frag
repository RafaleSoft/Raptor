#version 450 
layout (binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec4 v_Color;
layout(location = 1) in vec2 v_TexCoord;

layout(location = 0) out vec4 o_Color;

void main()
{
	//o_Color = v_Color;
	o_Color = texture(texSampler, v_TexCoord);
}
