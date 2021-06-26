#version 450

// ********** PARAMETERS & VARIABLES ***********

in vec2 textCoord;


uniform float extrusion;
uniform uint height, width;
uniform sampler2D texHeightSampler;


layout (location = 0) out vec4 fColor;


// Computes the normal map for a grayscale texture (1 channel) from sobel filter
// step_x and step_y are difference in X and Y between neighbors (texture coordinates, [0, 1])
vec3 computeGradientMap(const float step_x, const float step_y)
{
	const float tLeft = texture(texHeightSampler, textCoord + vec2(-step_x, step_y)).r;
	const float top = texture(texHeightSampler, textCoord + vec2(0.0f, step_y)).r;
	const float tRight = texture(texHeightSampler, textCoord + vec2(step_x, step_y)).r;

	const float left = texture(texHeightSampler, textCoord + vec2(-step_x, 0.0f)).r;
	const float right = texture(texHeightSampler, textCoord + vec2(step_x, 0.0f)).r;

	const float bLeft = texture(texHeightSampler, textCoord + vec2(-step_x, -step_y)).r;
	const float bottom = texture(texHeightSampler, textCoord + vec2(0.0f, -step_y)).r;
	const float bRight = texture(texHeightSampler, textCoord + vec2(step_x, -step_y)).r;

	// Sobel masks (http://en.wikipedia.org/wiki/Sobel_operator)
	//        1 0 -1     -1 -2 -1
	//    X = 2 0 -2  Y = 0  0  0
	//        1 0 -1      1  2  1

	const float x = tLeft - tRight + 2.0f * left - 2.0f * right + bLeft - bRight;
	const float y = -tLeft - 2.0f * top - tRight + bLeft + 2.0f * bottom + bRight;						// Until this line => sobel filter
	const float depth = 1.0f / extrusion;
	const float scale = 0.5f / sqrt(x * x + y * y + depth * depth);

	const float r = 0.5f + x * scale;
	const float g = 0.5f + y * scale;
	const float b = 0.5f + depth * scale;

	return vec3(r, g, b);
}


void main(void)
{
	const float step_x = 1.0f / width, step_y = 1.0f / height;
	fColor = vec4(computeGradientMap(step_x, step_y), 1.0f);
}
