#include "stdafx.h"
#include "OpenGLUtilities.h"

/// [Static members initialization]

std::unique_ptr<VAO> _cubeVAO;
std::unique_ptr<VAO> _quadVAO;

/// [Public methods]

std::vector<vec4> Primitives::getCubePoints(const vec3& minValues, const vec3& maxValues)
{
	const std::vector<vec4> points
	{
		vec4(minValues[0], minValues[1], maxValues[2], 1.0f),
		vec4(maxValues[0], minValues[1], maxValues[2], 1.0f),
		vec4(minValues[0], minValues[1], minValues[2], 1.0f),
		vec4(maxValues[0], minValues[1], minValues[2], 1.0f),
		vec4(minValues[0], maxValues[1], maxValues[2], 1.0f),
		vec4(maxValues[0], maxValues[1], maxValues[2], 1.0f),
		vec4(minValues[0], maxValues[1], minValues[2], 1.0f),
		vec4(maxValues[0], maxValues[1], minValues[2], 1.0f)
	};

	return points;
}

std::vector<GLuint> Primitives::getCubeWireframeIndices(const unsigned startIndex)
{
	const std::vector<GLuint> wireframe
	{
		startIndex + 0, startIndex + 1, Model3D::RESTART_PRIMITIVE_INDEX, startIndex + 0, startIndex + 2, Model3D::RESTART_PRIMITIVE_INDEX,
		startIndex + 1, startIndex + 3, Model3D::RESTART_PRIMITIVE_INDEX, startIndex + 2, startIndex + 3, Model3D::RESTART_PRIMITIVE_INDEX,
		startIndex + 4, startIndex + 5, Model3D::RESTART_PRIMITIVE_INDEX, startIndex + 4, startIndex + 6, Model3D::RESTART_PRIMITIVE_INDEX,
		startIndex + 5, startIndex + 7, Model3D::RESTART_PRIMITIVE_INDEX, startIndex + 6, startIndex + 7, Model3D::RESTART_PRIMITIVE_INDEX,
		startIndex + 0, startIndex + 4, Model3D::RESTART_PRIMITIVE_INDEX, startIndex + 1, startIndex + 5, Model3D::RESTART_PRIMITIVE_INDEX,
		startIndex + 2, startIndex + 6, Model3D::RESTART_PRIMITIVE_INDEX, startIndex + 3, startIndex + 7, Model3D::RESTART_PRIMITIVE_INDEX
	};

	return wireframe;
}


VAO* Primitives::getCubeVAO()
{
	if (!_cubeVAO.get())
	{
		const vec3 minValues(-0.5f), maxValues(0.5f);

		const std::vector<vec4> points = getCubePoints(minValues, maxValues);
		const std::vector<vec3> normals
		{
			glm::normalize(vec3(-0.5f, -0.5f, 0.5f)),
			glm::normalize(vec3(0.5f, -0.5f, 0.5f)),
			glm::normalize(vec3(-0.5f, -0.5f, -0.5f)),
			glm::normalize(vec3(0.5f, -0.5f, -0.5f)),
			glm::normalize(vec3(-0.5f, 0.5f, 0.5f)),
			glm::normalize(vec3(0.5f, 0.5f, 0.5f)),
			glm::normalize(vec3(-0.5f, 0.5f, -0.5f)),
			glm::normalize(vec3(0.5f, 0.5f, -0.5f))
		};
		const std::vector<vec2> textCoords{ vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f) };

		const std::vector<GLuint> pointCloud{ 0, 1, 2, 3, 4, 5, 6, 7 };
		const std::vector<GLuint> wireframe
		{ 
			0, 1, Model3D::RESTART_PRIMITIVE_INDEX, 0, 2, Model3D::RESTART_PRIMITIVE_INDEX, 
			1, 3, Model3D::RESTART_PRIMITIVE_INDEX, 2, 3, Model3D::RESTART_PRIMITIVE_INDEX,
			4, 5, Model3D::RESTART_PRIMITIVE_INDEX, 4, 6, Model3D::RESTART_PRIMITIVE_INDEX,
			5, 7, Model3D::RESTART_PRIMITIVE_INDEX, 6, 7, Model3D::RESTART_PRIMITIVE_INDEX,
			0, 4, Model3D::RESTART_PRIMITIVE_INDEX, 1, 5, Model3D::RESTART_PRIMITIVE_INDEX,
			2, 6, Model3D::RESTART_PRIMITIVE_INDEX, 3, 7, Model3D::RESTART_PRIMITIVE_INDEX
		};
		const std::vector<GLuint> triangleMesh
		{
			0, 1, 2, Model3D::RESTART_PRIMITIVE_INDEX, 1, 3, 2, Model3D::RESTART_PRIMITIVE_INDEX, 4, 5, 6, Model3D::RESTART_PRIMITIVE_INDEX,
			5, 7, 6, Model3D::RESTART_PRIMITIVE_INDEX, 0, 1, 4, Model3D::RESTART_PRIMITIVE_INDEX, 1, 5, 4, Model3D::RESTART_PRIMITIVE_INDEX,
			2, 0, 4, Model3D::RESTART_PRIMITIVE_INDEX, 2, 4, 6, Model3D::RESTART_PRIMITIVE_INDEX, 1, 3, 5, Model3D::RESTART_PRIMITIVE_INDEX,
			3, 7, 5, Model3D::RESTART_PRIMITIVE_INDEX, 3, 2, 6, Model3D::RESTART_PRIMITIVE_INDEX, 2, 6, 7, Model3D::RESTART_PRIMITIVE_INDEX
		};

		VAO* cubeVAO = new VAO();

		cubeVAO->setVBOData(RendEnum::VBO_POSITION, points);
		cubeVAO->setVBOData(RendEnum::VBO_NORMAL, normals);
		cubeVAO->setVBOData(RendEnum::VBO_TEXT_COORD, textCoords);

		cubeVAO->setIBOData(RendEnum::IBO_POINT_CLOUD, pointCloud);
		cubeVAO->setIBOData(RendEnum::IBO_WIREFRAME, wireframe);
		cubeVAO->setIBOData(RendEnum::IBO_TRIANGLE_MESH, triangleMesh);

		_cubeVAO.reset(cubeVAO);
	}

	return _cubeVAO.get();
}

VAO* Primitives::getCubesVAO(std::vector<Model3D::BVHCluster>& nodes, unsigned nNodes)
{
	VAO*				vao = new VAO();
	std::vector<vec4>	position;							// Positions for each node
	std::vector<GLuint>	wireframe;							// Topology array
	unsigned			numAvailableNodes = nodes.size();

	// Size is known from nNodes => reserve space
	position.reserve(nNodes * 8);
	wireframe.reserve(nNodes * 36);

	for (int cluster = 0; cluster < nNodes && cluster < numAvailableNodes; ++cluster)
	{
		const auto newCubePos = Primitives::getCubePoints(nodes[cluster]._minPoint, nodes[cluster]._maxPoint);
		const auto newCubeWireframe = Primitives::getCubeWireframeIndices(cluster * 8);

		position.insert(position.end(), newCubePos.begin(), newCubePos.end());
		wireframe.insert(wireframe.end(), newCubeWireframe.begin(), newCubeWireframe.end());
	}

	vao->setVBOData(RendEnum::VBOTypes::VBO_POSITION, position, GL_DYNAMIC_DRAW);
	vao->setIBOData(RendEnum::IBOTypes::IBO_WIREFRAME, wireframe, GL_DYNAMIC_DRAW);

	return vao;
}

VAO* Primitives::getNormalizedMeshVAO(unsigned numDivs, unsigned& numIndices)
{
	VAO* vao = new VAO();
	std::vector<vec4>	position;							// Positions for each node
	std::vector<GLuint>	wireframe;							// Topology array
	std::vector<GLuint>	triangleMesh;						// Topology array
	float n = numDivs;

	for (int x = 0; x <= numDivs; ++x)
	{
		for (int z = 0; z <= numDivs; ++z)
		{
			position.push_back(vec4(x / n, .0f, z / n, 1.0f));
		}
	}

	// Triangles
	unsigned numDivsPlusOne = numDivs + 1;
	for (int x = 0; x < numDivs; ++x)
	{
		for (int z = 0; z < numDivs; ++z)
		{
			triangleMesh.push_back(z * numDivsPlusOne + x);
			triangleMesh.push_back(z * numDivsPlusOne + x + 1);
			triangleMesh.push_back((z + 1) * numDivsPlusOne + x);
			triangleMesh.push_back(Model3D::RESTART_PRIMITIVE_INDEX);

			triangleMesh.push_back(z * numDivsPlusOne + x + 1);
			triangleMesh.push_back((z + 1) * numDivsPlusOne + x + 1);
			triangleMesh.push_back((z + 1) * numDivsPlusOne + x);
			triangleMesh.push_back(Model3D::RESTART_PRIMITIVE_INDEX);
		}
	}

	vao->setVBOData(RendEnum::VBOTypes::VBO_POSITION, position, GL_DYNAMIC_DRAW);
	vao->setIBOData(RendEnum::IBOTypes::IBO_WIREFRAME, triangleMesh, GL_DYNAMIC_DRAW);
	vao->setIBOData(RendEnum::IBOTypes::IBO_TRIANGLE_MESH, triangleMesh, GL_DYNAMIC_DRAW);

	numIndices = triangleMesh.size();

	return vao;
}

Model3D::ModelComponent* Primitives::getQuadModelComp()
{
	Model3D::ModelComponent* modelComp = new Model3D::ModelComponent(nullptr);
	Model3D::VertexGPUData vertex;
	Model3D::FaceGPUData face;

	// Geometry
	const std::vector<vec2> quadTextCoord{ vec2(-.5f, 0.0f), vec2(.5f, 0.0f), vec2(-.5f, 1.0f), vec2(.5f, 1.0f) };			// Quad centered in X axis, not in Y axis

	// Topology
	const std::vector<GLuint> triangleMesh{ 0, 1, 2, Model3D::RESTART_PRIMITIVE_INDEX, 1, 3, 2, Model3D::RESTART_PRIMITIVE_INDEX };				// Topology

	for (const vec2& textCoord : quadTextCoord)
	{
		vertex._position = vec3(textCoord, .0f);
		vertex._normal = vec3(.0f, .0f, 1.0f);
		vertex._tangent = vec3(1.0f, .0f, .0f);
		vertex._textCoord = vec2(vertex._position) + vec2(.5f, .0f);

		modelComp->_geometry.push_back(vertex);
	}
	
	for (int i = 0; i < triangleMesh.size(); i += 4)
	{
		face._vertices = uvec3(triangleMesh[i], triangleMesh[i + 1], triangleMesh[i + 2]);
		face._normal = modelComp->_geometry[triangleMesh[i]]._normal;

		modelComp->_topology.push_back(face);
	}

	modelComp->_triangleMesh.insert(modelComp->_triangleMesh.end(), triangleMesh.begin(), triangleMesh.end());				// Raw topology

	return modelComp;
}

VAO* Primitives::getQuadVAO()
{
	if (!_quadVAO.get())
	{
		const std::vector<vec2> quadTextCoord{ vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f) };							// From texture coordinates we can compute position
		const std::vector<GLuint> triangleMesh{ 0, 1, 2, Model3D::RESTART_PRIMITIVE_INDEX, 1, 3, 2, Model3D::RESTART_PRIMITIVE_INDEX };				// Topology

		VAO* quadVAO = new VAO();
		quadVAO->setVBOData(RendEnum::VBO_TEXT_COORD, quadTextCoord);
		quadVAO->setIBOData(RendEnum::IBO_TRIANGLE_MESH, triangleMesh);

		_quadVAO.reset(quadVAO);
	}

	return _quadVAO.get();
}

Model3D::ModelComponent* Primitives::getSphereModelComp(const unsigned sectors, const unsigned stacks, const float radius)
{
	Model3D::ModelComponent* modelComp = new Model3D::ModelComponent(nullptr);
	std::vector<Model3D::VertexGPUData> geometry;

	vec3 position;                             
	vec3 normal;
	vec2 textCoord;
	float xy, lengthInv = 1.0f / radius;		

	float sectorStep = 2 * M_PI / sectors;
	float stackStep = M_PI / stacks;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stacks; ++i)
	{
		stackAngle = M_PI / 2.0f - i * stackStep;			// Starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);						// r * cos(u)
		position.z = radius * sinf(stackAngle);				// r * sin(u)

		// Add (sectorCount + 1) vertices per stack the first and last vertices have same position and normal, but different texture coordinates
		for (int j = 0; j <= sectors; ++j)
		{
			sectorAngle = j * sectorStep;					// Starting from 0 to 2pi

			// Vertex position (x, y, z)
			position = vec3(xy * cosf(sectorAngle), xy * sinf(sectorAngle), position.z);							// r * cos(u) * cos(v), r * cos(u) * sin(v), r * sin(u)
			normal = vec3(position.x * lengthInv, position.y * lengthInv, position.z * lengthInv);
			textCoord = vec2((float) j / sectors, (float) i / stacks);

			Model3D::VertexGPUData vertexData;
			vertexData._position = position;
			vertexData._normal = normal;
			vertexData._textCoord = textCoord;
			vertexData._tangent = vec3(-sin(sectorAngle), .0f, -cos(sectorAngle));

			geometry.push_back(vertexData);
		}
	}

	int k1, k2, topologySize = 0;

	for (int i = 0; i < stacks; ++i)
	{
		k1 = i * (sectors + 1);     // Beginning of current stack
		k2 = k1 + sectors + 1;      // Beginning of next stack

		for (int j = 0; j < sectors; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				modelComp->_geometry.push_back(geometry[k1]);
				modelComp->_geometry.push_back(geometry[k2]);
				modelComp->_geometry.push_back(geometry[k1 + 1]);

				modelComp->_triangleMesh.push_back(k1);
				modelComp->_triangleMesh.push_back(k2);
				modelComp->_triangleMesh.push_back(k1 + 1);
				modelComp->_triangleMesh.push_back(Model3D::RESTART_PRIMITIVE_INDEX);

				modelComp->_topology.push_back(Model3D::FaceGPUData());
				modelComp->_topology[topologySize++]._vertices = uvec3(k1, k2, k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stacks - 1))
			{
				modelComp->_geometry.push_back(geometry[k1 + 1]);
				modelComp->_geometry.push_back(geometry[k2]);
				modelComp->_geometry.push_back(geometry[k2 + 1]);

				modelComp->_triangleMesh.push_back(k1 + 1);
				modelComp->_triangleMesh.push_back(k2);
				modelComp->_triangleMesh.push_back(k2 + 1);
				modelComp->_triangleMesh.push_back(Model3D::RESTART_PRIMITIVE_INDEX);

				modelComp->_topology.push_back(Model3D::FaceGPUData());
				modelComp->_topology[topologySize++]._vertices = uvec3(k1 + 1, k2, k2 + 1);
			}
		}
	}

	//modelComp->_geometry = geometry;

	return modelComp;
}
