#version 330

in vec2 uv;
in vec4 color;
in float radius;

out vec4 finalColor;

void main()
{
	float dist = length(uv - vec2(0.5));
	if (dist < 0.5)
		finalColor = color;
	else
		discard;
}
