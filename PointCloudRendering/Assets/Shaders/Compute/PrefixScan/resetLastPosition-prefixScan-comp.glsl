#version 450

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

layout (std430, binding = 0) buffer InputBuffer { uint inBuffer[]; };

uniform uint arraySize;


void main()
{
	inBuffer[arraySize - 1] = 0;
}