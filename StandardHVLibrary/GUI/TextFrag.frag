#version 420

layout(location = 0) in vec2 i_uv;
layout(location = 1) in vec4 i_color;

layout(location = 0) out vec4 out_Color;

layout(set=0,binding=0) uniform sampler2D u_Texture;

void main()
{
	out_Color = texture(u_Texture,i_uv)*i_color;
}

