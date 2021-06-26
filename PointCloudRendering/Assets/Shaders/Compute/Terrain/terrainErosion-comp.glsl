#version 450

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

layout (std430, binding = 0) buffer MapBuffer			{ int map[]; };
layout (std430, binding = 1) buffer DropBuffer			{ vec4 dropPosition[]; };
layout (std430, binding = 2) buffer BrushIndicesBuffer	{ int brushIndices[]; };
layout (std430, binding = 3) buffer BrushWeightsBuffer	{ float brushWeights[]; };

#define EPSILON 0.000001f
#define FLOAT_MULTIPLIER 100000.0f

// Map parameters
uniform uint	mapSize;
uniform uint	numParticles;
uniform uint	brushLength;
uniform uint	borderSize;

// Erosion settings
uniform float	depositSpeed;
uniform float	erodeSpeed;
uniform float	evaporateSpeed;
uniform float	gravity;
uniform float	inertia;				
uniform uint	maxLifetime;
uniform float	minSedimentCapacity;
uniform float	sedimentCapacityFactor;
uniform float	startSpeed;
uniform float	startWater;	

// Gradient (x, y) and gradient for a new pos
vec3 calculateHeightGradient(vec2 pos)
{
	ivec2 node_pos = ivec2(pos);
	vec2 offset = pos - vec2(node_pos);

	// Calculate height of boundaries
	uint nodeIndexNW = node_pos.y * mapSize + node_pos.x;
	float heightNW = map[nodeIndexNW] / FLOAT_MULTIPLIER;
	float heightNE = map[nodeIndexNW + 1] / FLOAT_MULTIPLIER;
	float heightSW = map[nodeIndexNW + mapSize] / FLOAT_MULTIPLIER;
	float heightSE = map[nodeIndexNW + mapSize + 1] / FLOAT_MULTIPLIER;

	// Compute gradient
	float gradient_x = (1.0f - offset.y) * (heightNE - heightNW) + offset.y * (heightSE - heightSW);
	float gradient_y = (1.0f - offset.x) * (heightSW - heightNW) + offset.x * (heightSE - heightNE);
	float height = heightNW * (1.0f - offset.x) * (1.0f - offset.y) + heightNE * offset.x * (1.0f - offset.y) + heightSW * (1.0f - offset.x) * offset.y + heightSE * offset.x * offset.y;

	return vec3(gradient_x, gradient_y, height);
}

void main()
{
	const uint threadIndex = gl_GlobalInvocationID.x;
	if (threadIndex >= numParticles)
	{
		return;
	}

	vec2 pos = dropPosition[threadIndex].xy;
	vec2 dir = vec2(.0f);
	vec3 currentGradientHeight = calculateHeightGradient(pos);
	float speed = startSpeed;
	float water = startWater;
	float sediment = 0;

	for (int it = 0; it < maxLifetime; ++it)
	{
		ivec2 node_pos = ivec2(pos);
		uint dropletIndex = node_pos.y * mapSize + node_pos.x;

		// Dropplet inside cell
		vec2 offset = pos - vec2(node_pos);

		// Update position (move 1 unit regardless of speed)
		dir.x = dir.x * inertia - currentGradientHeight.x * (1.0f - inertia);
		dir.y = dir.y * inertia - currentGradientHeight.y * (1.0f - inertia);
		dir = normalize(dir);

		// Update position with new direction
		pos += dir;

		// Condition of stop (not movement or has exceeded edges of map)
		if ((length(dir) < EPSILON) || pos.x < borderSize || pos.x > mapSize - borderSize || pos.y < borderSize || pos.y > mapSize - borderSize)
		{
			break;
		}

		// Find new height and compute height variation
		vec3 newGradientHeight = calculateHeightGradient(pos);
		float newHeight = newGradientHeight.z;
		float deltaHeight = newHeight - currentGradientHeight.z;

		// New sediment capacity. Higher when carrying more water, moving faster or going downhill
		float sedimentCapacity = max(-deltaHeight * speed * water * sedimentCapacityFactor, minSedimentCapacity);

		// Carrying more sediment than actual capacity or going uphill
		if (sediment > sedimentCapacity || deltaHeight > .0f)
		{
			// Try to fill up current height or deposit a fraction of excess sediment
			float amountToDeposit = (deltaHeight > .0f) ? min(deltaHeight, sediment) : (sediment - sedimentCapacity) * depositSpeed;
			sediment -= amountToDeposit;

			// Add the sediment to four nodes from the current cell by bilinear interpolation
			// Deposition is not distributed over a radius (like erosion) so that it can fill small pits
			atomicAdd(map[dropletIndex], int(FLOAT_MULTIPLIER * (amountToDeposit * (1.0f - offset.x) * (1.0f - offset.y))));
			atomicAdd(map[dropletIndex + 1], int(FLOAT_MULTIPLIER * (amountToDeposit * offset.x * (1.0f - offset.y))));
			atomicAdd(map[dropletIndex + mapSize], int(FLOAT_MULTIPLIER * (amountToDeposit * (1.0f - offset.x) * offset.y)));
			atomicAdd(map[dropletIndex + mapSize + 1], int(FLOAT_MULTIPLIER * (amountToDeposit * offset.x * offset.y)));
		}
		else
		{
			// Erode a fraction of the droplet's current capacity 
			// Clamp height to avoid digging a hole in the terrain behind the droplet
			float amountToErode = min((sedimentCapacity - sediment) * erodeSpeed, -deltaHeight);

			for (int i = 0; i < brushLength; ++i)
			{
				uint erodeIndex = dropletIndex + brushIndices[i];

				float weightedErodeAmount = amountToErode * brushWeights[i];
				float heightErodeIndex = map[erodeIndex] / FLOAT_MULTIPLIER;
				float deltaSediment = (heightErodeIndex < weightedErodeAmount) ? heightErodeIndex : weightedErodeAmount;
				atomicAdd(map[erodeIndex], int(-deltaSediment * FLOAT_MULTIPLIER));
				sediment += deltaSediment;
			}
		}

		// Update drop parameters
		speed = sqrt(max(.0f, speed * speed + deltaHeight * gravity));
		water *= (1.0f - evaporateSpeed);
		currentGradientHeight = newGradientHeight;
	}
}