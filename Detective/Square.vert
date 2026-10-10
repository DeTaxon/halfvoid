#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(push_constant) uniform PushConsts{
	vec2 screenScales;
}consts;

layout (location = 0) out vec2 outUV;


vec2 mesh[4] = {
	vec2(-1.0,-1.0),
	vec2(1.0,-1.0),
	vec2(1.0,1.0),
	vec2(-1.0,1.0)
};
void main() 
{
	outUV = mesh[gl_VertexIndex];
	gl_Position = vec4(outUV, 0.0f, 1.0f);
	gl_Position.w = 1.05;
	// outUV *= consts.screenScales;
}
