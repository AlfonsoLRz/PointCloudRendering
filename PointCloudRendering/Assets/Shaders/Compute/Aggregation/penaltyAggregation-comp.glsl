#version 450

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

#define NUM_AGGREGATIONS 5

layout(std430, binding = 0) buffer SourceBuffer		{ vec4 sourceImg[]; };
layout(std430, binding = 1) buffer TemplateBuffer	{ vec4 templateImg[]; };

// ------------- Aggregation function ----------------
subroutine vec4 penaltyType(uvec2 threadIndex);
subroutine uniform penaltyType penaltyUniform;

uniform int height, width;
uniform int scale;

void order(inout vec3 a, inout vec3 b)
{
	if (a.x > b.x) 
	{
		float tmp = a.x; a.x = b.x; b.x = tmp;
	}
	
	if (a.y > b.y) 
	{ 
		float tmp = a.y; a.y = b.y; b.y = tmp; 
	}
	
	if (a.z > b.z) 
	{ 
		float tmp = a.z; a.z = b.z; b.z = tmp; 
	}
}

uint getPosition(uvec2 threadIndex, uvec2 localIndex)
{
	return (threadIndex.x * scale + localIndex.x) + (threadIndex.y * scale + localIndex.y) * width * scale;
}

vec4 minimum(uvec2 threadIndex)
{
	vec4 minimum = vec4(1.0f);

	for (int x = 0; x < scale; ++x)
	{
		for (int y = 0; y < scale; ++y)
		{
			minimum = min(minimum, sourceImg[getPosition(threadIndex, uvec2(x, y))]);
		}
	}

	return minimum;
}

vec4 arithmeticMean(uvec2 threadIndex)
{
	vec4 sum = vec4(.0f);

	for (int x = 0; x < scale; ++x)
	{
		for (int y = 0; y < scale; ++y)
		{
			sum += sourceImg[getPosition(threadIndex, uvec2(x, y))];
		}
	}

	return sum / (scale * scale);
}

vec4 geometricMean(uvec2 threadIndex)
{
	vec4 mult = vec4(1.0f);
	float exponent = scale * scale;

	for (int x = 0; x < scale; ++x)
	{
		for (int y = 0; y < scale; ++y)
		{
			mult *= sourceImg[getPosition(threadIndex, uvec2(x, y))];
		}
	}

	return vec4(pow(mult.x, 1.0f / exponent), pow(mult.y, 1.0f / exponent), pow(mult.z, 1.0f / exponent), 1.0f);
}

vec4 median(uvec2 threadIndex)
{
	const uint size = scale * scale;
	const uint medianIndex = (size + 1) / 2;			// Odd number
	vec3 orderedArray[9];
	uint x, y, index;

	for (int n = 0; n < size - 1; ++n)
	{
		y = uint(floor(n / 3)); x = n % 3;
		orderedArray[n] = sourceImg[getPosition(threadIndex, uvec2(x, y))].xyz;
	}

	for (int it1 = 0; it1 < size - 1; ++it1)
	{
		for (int it2 = it1; it2 < size - 1; ++it2)   
		{
			order(orderedArray[it2], orderedArray[it2 + 1]);
		}
	}

	return vec4(orderedArray[medianIndex], 1.0f);
}

vec4 maximum(uvec2 threadIndex)
{
	vec4 maximum = vec4(0.0f);

	for (int x = 0; x < scale; ++x)
	{
		for (int y = 0; y < scale; ++y)
		{
			maximum = max(maximum, sourceImg[getPosition(threadIndex, uvec2(x, y))]);
		}
	}

	return maximum;
}

// ----- Penalty function -------

subroutine(penaltyType)
vec4 arithmeticMeanPenalty(uvec2 threadIndex)
{
	uint selectedAggregation = 0;
	float minPenalty = 500000.0f;

	vec3 aggregationResults[NUM_AGGREGATIONS];
	float penaltyResults[NUM_AGGREGATIONS];
	aggregationResults[0] = minimum(threadIndex).xyz;
	aggregationResults[1] = arithmeticMean(threadIndex).xyz;
	aggregationResults[2] = geometricMean(threadIndex).xyz;
	aggregationResults[3] = median(threadIndex).xyz;
	aggregationResults[4] = maximum(threadIndex).xyz;

	for (int aggr = 0; aggr < NUM_AGGREGATIONS; ++aggr)
	{
		penaltyResults[aggr] = .0f;

		for (int x = 0; x < scale; ++x)
		{
			for (int y = 0; y < scale; ++y)
			{
				vec3 difference = aggregationResults[aggr] - sourceImg[getPosition(threadIndex, uvec2(x, y))].xyz;
				penaltyResults[aggr] = pow(difference.x, 2.0f) + pow(difference.y, 2.0f) + pow(difference.z, 2.0f);
			}
		}
	}

	for (int aggr = 0; aggr < NUM_AGGREGATIONS; ++aggr)
	{
		if (minPenalty > penaltyResults[aggr])
		{
			minPenalty = penaltyResults[aggr];
			selectedAggregation = aggr;
		}
	}

	float normalizedValue = selectedAggregation / 5.0f;

	return vec4(aggregationResults[selectedAggregation], 1.0f);
}


void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= height * width) return;

	uvec2 threadIndex = uvec2(index % width, index / width);
	vec2 normalized = threadIndex / vec2(width, height);

	templateImg[index] = penaltyUniform(threadIndex);
}