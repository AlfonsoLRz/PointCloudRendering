#version 450

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTextCoord;
layout(location = 4) in vec4 vOffset;
layout(location = 5) in vec4 vScale;
layout(location = 6) in vec4 vRotation;

// Matrices
uniform mat4 mModelView;
uniform mat4 mModelViewProj;

out vec3 normal;
out vec2 textCoord;


void main()
{
	mat4 scaleMatrix = mat4(vScale.x, .0f, .0f, .0f, .0f, vScale.y, .0f, .0f, .0f, .0f, vScale.z, .0f, .0f, .0f, .0f, 1.0f);
	mat4 translationMatrix = mat4(1.0f, .0f, .0f, .0f, .0f, 1.0f, .0f, .0f, .0f, .0f, 1.0f, .0f, vOffset.x, vOffset.y, vOffset.z, 1.0f);
	mat4 rotationXMatrix = mat4(1.0f, .0f, .0f, .0f, .0f, cos(vRotation.x), sin(vRotation.x), .0f, .0f, -sin(vRotation.x), cos(vRotation.x), .0f, .0f, .0f, .0f, 1.0f);
	mat4 rotationYMatrix = mat4(cos(vRotation.y), .0f, -sin(vRotation.y), .0f, .0f, 1.0f, .0f, .0f, sin(vRotation.y), .0f, cos(vRotation.y), .0f, .0f, .0f, .0f, 1.0f);
	mat4 rotationZMatrix = mat4(cos(vRotation.z), sin(vRotation.z), .0f, .0f, -sin(vRotation.z), cos(vRotation.z), .0f, .0f, .0f, .0f, 1.0f, .0f, .0f, .0f, .0f, 1.0f);
	mat4 transformationMatrix = translationMatrix * scaleMatrix * rotationZMatrix * rotationYMatrix * rotationXMatrix;

	normal = vec3(mModelView * transformationMatrix * vec4(vNormal, 0.0f));
	textCoord = vTextCoord;

	gl_Position = mModelViewProj * transformationMatrix * vec4(vPosition, 1.0f);
}
