#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(push_constant) uniform PushConsts{
	vec2 scale;
	vec2 offset;
}consts;

layout (location = 0) out vec2 outUV;


vec2 mesh[4] = {
	vec2(0.0,0.0),
	vec2(1.0,0.0),
	vec2(1.0,1.0),
	vec2(0.0,1.0)
};
void main() 
{
	outUV = mesh[gl_VertexIndex];
	vec2 pos = outUV*consts.scale + consts.offset;
	pos = pos*2.0 - 1.0;
	gl_Position = vec4(pos, 0.0f, 1.0f);
	outUV = outUV*2.0 - 1.0;
	// outUV *= consts.screenScales;
}
