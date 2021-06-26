#version 450

layout (location = 0) in vec4 vPosition;

uniform mat4 mModelViewProj;
uniform float pointSize;				

void main() {
	gl_PointSize = pointSize;
	gl_Position = mModelViewProj * vPosition;
}