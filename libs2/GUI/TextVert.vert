#version 420

layout(location = 0) in vec2 i_Position;
layout(location = 1) in vec2 i_uv;
layout(location = 2) in vec4 i_Color;

layout(location = 0) out vec2 o_uv;
layout(location = 1) out vec4 o_color;

layout(push_constant) uniform PushConsts{
	vec2 screenResolution;
}consts;

void main() {
	gl_Position = vec4(i_Position,0.0f,1.0f)*vec4(consts.screenResolution,0.0,1.0);
	gl_Position -= vec4(0.5,0.5,0.0,0.0);
	gl_Position *= vec4(2.0,2.0,1.0,1.0);
	//gl_Position.y = -gl_Position.y;
	o_color = i_Color;//*vec4(1/255.0,1/255.0,1/255.0,1/255.0);
	o_uv = i_uv;//vec2(i_uv.x,1.0 - i_uv.y);
}
