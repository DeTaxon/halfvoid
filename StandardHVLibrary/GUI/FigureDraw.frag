
#version 420

layout(location = 0) in vec4 i_color;

layout(location = 0) out vec4 out_Color;

void main()
{
	out_Color = i_color;
}
