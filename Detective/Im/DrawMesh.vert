#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec2 i_pos;
layout(location = 1) in vec2 i_uv;

layout(push_constant) uniform PushConsts{
	vec2 screenScales;
}consts;

layout (location = 0) out vec2 outUV;

void main() 
{
	gl_Position = vec4(i_pos, 0.0f, 1.0f);
	outUV = i_uv;
	
	// outUV *= consts.screenScales;
}
