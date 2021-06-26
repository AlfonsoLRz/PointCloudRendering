#version 450

// ********** PARAMETERS & VARIABLES ***********

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTextCoord;
layout(location = 3) in vec3 vTangent;

#define DISPLACEMENT_LENGTH .008f
#define TILING 6.0f

// ------------- Light types ----------------
subroutine void lightType(const mat3 TBN);
subroutine uniform lightType lightUniform;

// ------------- Displacement type ---------------
subroutine mat3 displacementType();
subroutine uniform displacementType displacementUniform;

// Lighting
uniform vec3 lightPosition;
uniform vec3 lightDirection;

// Matrices
uniform mat4 mModelView;
uniform mat4 mModelViewProj;
uniform mat4 mShadow;

// Water textures
uniform sampler2D texReflectionNoise;

// Animation
uniform float t;


// Vertex related
out vec3 position;
out vec3 normal;
out vec2 textCoord;
out vec4 shadowCoord;
out vec2 inReflectionTextCoord;

// Lighting related
out vec3 lightPos;
out vec3 lightDir;
out vec3 vertexToLightDir;
out vec3 viewDir;

// Water projection related
out vec4 clipSpace;


// ********* FUNCTIONS ************

// Computes (Tangent, Binormal, Normal) matrix. Note: the out parameter normal
// is considered as already computed
mat3 getTBN()
{
	const vec3 tangent = vec3(mModelView * vec4(vTangent, 0.0f));
	const vec3 binormal = normalize(cross(normal, tangent));

	return transpose(mat3(tangent, binormal, normal));
}

// ------------- Lighting --------------

// Computes out parameters taking into account the type of light we're treating
// Note: the out parameter position is considered as already computed

subroutine(lightType)
void ambientLight(const mat3 TBN)
{
	// Ambient light is computed taking into account no vector
}

subroutine(lightType)
void pointLight(const mat3 TBN)
{
	lightPos = TBN * lightPosition;
	vertexToLightDir = normalize(TBN * (lightPosition - position));
	viewDir = normalize(TBN * -position);

	// Point light has no light direction!
}

subroutine(lightType)
void directionalLight(const mat3 TBN)
{
	viewDir = normalize(TBN * -position);
	vertexToLightDir = normalize(TBN * -lightDirection);
}

subroutine(lightType)
void spotLight(const mat3 TBN)
{
	pointLight(TBN);
	lightDir = normalize(TBN * lightDirection);
}

subroutine(lightType)
void rimLight(const mat3 TBN)
{
	// Space has no power here!
}

// ------------- Textures --------------

// Displacement textures, no matters it is real, such as displacement mapping,
// or just figured, as bump mapping (doesn't translate but looks like that)

subroutine(displacementType)
mat3 bumpMappingDisplacement()
{
	return getTBN();				// We can't displace but make it looks like
}

subroutine(displacementType)
mat3 displacementMapping()
{
	return getTBN();
}

subroutine(displacementType)
mat3 noDisplacement()
{
	return mat3(1.0f);
}


void main()
{
	inReflectionTextCoord = vec2(vTextCoord.x, vTextCoord.y) * TILING;

	vec2 distortedTextCoord = texture(texReflectionNoise, vec2(inReflectionTextCoord.x + t, inReflectionTextCoord.y)).rg * 0.1f;
	distortedTextCoord = inReflectionTextCoord + vec2(distortedTextCoord.x, distortedTextCoord.y + t);
	vec2 totalDistortion = (texture(texReflectionNoise, distortedTextCoord).rg * 2.0f - 1.0f);
	vec3 newPosition = vPosition + vNormal * totalDistortion.x * DISPLACEMENT_LENGTH;

	position = vec3(mModelView * vec4(newPosition, 1.0f));
	clipSpace = mModelViewProj * vec4(newPosition, 1.0f);
	gl_Position = clipSpace;
	normal = vec3(mModelView * vec4(vNormal, 0.0f));			// Needed for TBN matrix
	shadowCoord = mShadow * vec4(newPosition, 1.0f);
	textCoord = vTextCoord;

	const mat3 TBN = displacementUniform();
	lightUniform(TBN);
}
