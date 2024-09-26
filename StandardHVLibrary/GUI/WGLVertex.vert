

layout(location = 0) in vec2 iPosition;
layout(location = 1) in vec2 iUV;
layout(location = 2) in vec4 iColor;

out vec4 FragColor;
out vec2 FragUV;

void main()
{
	oColor = iColor;
	gl_Position = vec4(iPosition,1.0,1.0);
	FragUV = iUV;
}
