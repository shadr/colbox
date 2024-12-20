#version 100

attribute vec2 pos;
attribute vec2 offset;
attribute float aRadius;
attribute vec4 aColor;

uniform mat4 viewproj;

varying vec2 uv;
varying vec4 color;
varying float radius;

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
