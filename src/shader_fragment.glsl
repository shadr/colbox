#version 100

varying mediump vec2 uv;
varying mediump vec4 color;
varying mediump float radius;

varying mediump vec4 finalColor;

void main()
{
	mediump float dist = length(uv - vec2(0.5));
	if (dist < 0.5)
		gl_FragColor = color;
	else
		discard;
}
