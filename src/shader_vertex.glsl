#version 330

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 offset;
layout (location = 2) in float aRadius;
layout (location = 3) in vec4 aColor;

uniform mat4 viewproj;

out vec2 uv;
out vec4 color;
out float radius;

void main()
{
	mat4 m;
	m[0][0] = aRadius;
	m[1][1] = aRadius;
	m[2][2] = 1.0;
	m[3] = vec4(offset, 0.0, 1.0);
	vec4 p = viewproj * m * vec4(pos, 0.0, 1.0);
	gl_Position = p;
	uv = (pos * 0.5 + 0.5).xy;
	color = aColor;
	radius = aRadius;
}
