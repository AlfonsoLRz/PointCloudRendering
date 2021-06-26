#version 450

// ********** PARAMETERS & VARIABLES ***********

const float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);			// Filter matrix weights (5x5)


in vec2 textCoord;


uniform int filterSize;
uniform sampler2D texBrightImage;					

subroutine void directionType(inout vec3 color, const vec2 textOffset);
subroutine uniform directionType directionUniform; 


layout (location = 0) out vec4 fColor;


// ********* FUNCTIONS ************

// Picks the direction where the blur filter is used. Receives the bloom color and the amount of movement in each direction

subroutine(directionType)
void horizontalDirection(inout vec3 color, const vec2 textOffset)
{
	for (int i = 1; i < filterSize; ++i)					// Starts by 1 since the 0 weights corresponds to the fragment position
    {
		color += texture(texBrightImage, textCoord + vec2(textOffset.x * i, 0.0f)).rgb * weight[i];
        color += texture(texBrightImage, textCoord - vec2(textOffset.x * i, 0.0f)).rgb * weight[i];
    }
}

subroutine(directionType)
void verticalDirection(inout vec3 color, const vec2 textOffset)
{
	for (int i = 1; i < filterSize; ++i)
    {
		color += texture(texBrightImage, textCoord + vec2(0.0f, textOffset.y * i)).rgb * weight[i];
        color += texture(texBrightImage, textCoord - vec2(0.0f, textOffset.y * i)).rgb * weight[i];
    }
}


void main()
{             
    vec2 textOffset = 1.0f / textureSize(texBrightImage, 0);				// Size of texel. Square image
	vec3 color = texture(texBrightImage, textCoord).rgb * weight[0];
	directionUniform(color, textOffset);

	fColor = vec4(color.rgb, 1.0f);
}