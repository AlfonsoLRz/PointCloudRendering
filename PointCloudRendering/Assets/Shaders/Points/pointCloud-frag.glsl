#version 450

uniform vec3 vColor;

layout (location = 0) out vec4 fColor;
layout (location = 1) out vec4 brightColor;

void main() {
	// Rounded points
	vec2 centerPointv = gl_PointCoord - 0.5f;
	if (dot(centerPointv, centerPointv) > 0.25f)		// Vector * vector = square module => we avoid square root
	{
		discard;										// Discarded because distance to center is bigger than 0.5
	}

	fColor = vec4(vColor, 1.0f);
	brightColor = vec4(0.0f);
}