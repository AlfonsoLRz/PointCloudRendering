#include "stdafx.h"
#include "TriangleMesh.h"

#include "Geometry/3D/Intersections3D.h"
#include "Utilities/ChronoUtilities.h"

/// [Public methods]

TriangleMesh::TriangleMesh(const std::string& objFilename)
{
	if (this->loadOBJ(objFilename))
	{
		this->computeTangents();
	}
}

TriangleMesh::TriangleMesh()
{
}

TriangleMesh::TriangleMesh(const TriangleMesh& mesh) 
{
	this->copyAttributes(mesh);
}

TriangleMesh::~TriangleMesh()
{
}

void TriangleMesh::classify(Plane& plane)
{
	Face::FacePlaneRelation relation;				// Data for each iteration
	Segment3D segment;
	vec3 point;

	for (int i = 0; i < _face.size(); ++i)
	{
		Triangle3D triangle = _face[i]._triangle;
		Intersections3D::intersect(triangle, plane, relation, segment, point);
	}
}

void TriangleMesh::computeTangents()
{
	vec3* tan1 = new vec3[_position.size()];
	memset(tan1, 0, _position.size() * sizeof(vec3));
	const int numTriangles = _face.size();

	for (long i = 0; i < numTriangles; i++)
	{
		const long meshIndex = i * 4;
		const long index1 = _face[i]._index[0], index2 = _face[i]._index[1], index3 = _face[i]._index[2];

		const vec3& v1 = _position[index1], v2 = _position[index2], v3 = _position[index3];
		const vec2& w1 = _textCoord[index1], w2 = _textCoord[index2], w3 = _textCoord[index3];

		const float x1 = v2.x - v1.x, x2 = v3.x - v1.x;
		const float y1 = v2.y - v1.y, y2 = v3.y - v1.y;
		const float z1 = v2.z - v1.z, z2 = v3.z - v1.z;

		const float s1 = w2.x - w1.x, s2 = w3.x - w1.x;
		const float t1 = w2.y - w1.y, t2 = w3.y - w1.y;

		const float r = 1.0f / (s1 * t2 - s2 * t1);
		const vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);

		tan1[index1] += sdir;
		tan1[index2] += sdir;
		tan1[index3] += sdir;
	}

	_tangent = std::vector<vec3>(_position.size());									// Resize

	for (long i = 0; i < _position.size(); i++)
	{
		const vec3& n = _normal[i];
		const vec3& t = tan1[i];

		_tangent[i] = glm::normalize(t - n * glm::dot(n, t));						// Gram-Schmidt orthogonalize
	}

	delete[] tan1;
}

vec3 TriangleMesh::getVertex(int i) const
{
	return (i < _position.size()) ? _position[i] : vec3();
}

TriangleMesh& TriangleMesh::operator=(const TriangleMesh& mesh)
{
	if (this != &mesh)
	{
		this->copyAttributes(mesh);
	}

	return *this;
}

bool TriangleMesh::pointInMesh(const vec3& point)
{
	Ray3D ray_01(point, point + vec3(0.0f, 1.0f, 0.0f));
	Ray3D ray_02(point, point + vec3(1.0f, 0.0f, 0.0f));
	Ray3D ray_03(point, point + vec3(0.0f, 0.0f, 1.0f));
	std::vector<Triangle3D> triangle;
	std::vector<vec3> points;

	int oddIntersections = 0;
	rayTraversalExh(ray_01, points, triangle);
	if (triangle.size() % 2 == 1) ++oddIntersections;
	triangle.clear();
	points.clear();

	rayTraversalExh(ray_02, points, triangle);
	if (triangle.size() % 2 == 1) ++oddIntersections;

	if (oddIntersections == 1)					// Odd number of intersections, throw a third one to break the question
	{
		triangle.clear();
		points.clear();

		rayTraversalExh(ray_03, points, triangle);
		if (!(triangle.size() % 2 == 1))
		{
			return false;
		}
	}
	else if (oddIntersections == 0)				// No intersections
	{
		return false;
	}

	return true;
}

Triangle3D* TriangleMesh::pushBackFace(const unsigned i1, const unsigned i2, const unsigned i3)
{
	_face.push_back(Face(i1, i2, i3, this));

	return &_face[_face.size() - 1]._triangle;
}

size_t TriangleMesh::pushBackVertex(const vec3& position, const vec3& normal, const vec2& textCoord, const vec3& tangent)
{
	_position.push_back(vec4(position, 1.0f));
	_normal.push_back(normal);
	_textCoord.push_back(textCoord);
	_tangent.push_back(tangent);

	this->_aabb.update(position);

	return _position.size() - 1;
}

bool TriangleMesh::rayTraversalExh(Ray3D& ray, std::vector<vec3>& point, std::vector<Triangle3D>& triangle)
{
	for (int i = 0; i < _face.size(); ++i)
	{
		Triangle3D tri(_position[_face[i].getVertexIndex(0)], _position[_face[i].getVertexIndex(1)], _position[_face[i].getVertexIndex(2)]);
		vec3 intersection;

		if (Intersections3D::intersect(tri, ray, intersection))
		{
			triangle.push_back(tri);
			point.push_back(intersection);
		}
	}

	return point.size() > 0;
}

/// [Protected methods]

void TriangleMesh::copyAttributes(const TriangleMesh& mesh)
{
	this->_position		= mesh._position;
	this->_normal		= mesh._normal;
	this->_tangent		= mesh._tangent;
	this->_textCoord	= mesh._textCoord;
	this->_face			= mesh._face;

	this->_aabb			= mesh._aabb;

	for (int i = 0; i < _face.size(); ++i)
	{
		_face[i]._triangleMesh = this;
	}
}

bool TriangleMesh::loadOBJ(const std::string& filename)
{
	FILE* file = nullptr; errno_t error;

	if ((error = fopen_s(&file, filename.c_str(), "r")) != 0)
	{
		std::cout << "The file could not be opened!" << std::endl;

		return false;
	}

	std::vector<glm::vec3> tempNormals;						// Temporal data for the loading process
	std::vector<glm::vec2> tempTextures;
	vec3 normal;
	vec2 textCoord;

	int result;
	char lineHeader[128];
	unsigned int incorrectPrimitives = 0;

	while ((result = fscanf_s(file, "%s", lineHeader, sizeof(lineHeader))) != EOF)				// Type of geometry we are receiving
	{
		if (strcmp(lineHeader, "v") == 0)					// Vertex
		{
			vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);

			_position.push_back(vec4(vertex, 1.0f));
			_normal.resize(_position.size());
			_textCoord.resize(_position.size());

			_aabb.update(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)				// Texture coordinate
		{
			fscanf_s(file, "%f %f\n", &textCoord.x, &textCoord.y);

			tempTextures.push_back(textCoord);
		}
		else if (strcmp(lineHeader, "vn") == 0)				// Normal
		{
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);

			tempNormals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)				// Triangle mesh face indices
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], textureIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&vertexIndex[0], &textureIndex[0], &normalIndex[0],
				&vertexIndex[1], &textureIndex[1], &normalIndex[1],
				&vertexIndex[2], &textureIndex[2], &normalIndex[2]);

			if (matches != 9) {
				++incorrectPrimitives;
			}

			_face.push_back(Face(vertexIndex[0] - 1, vertexIndex[1] - 1, vertexIndex[2] - 1, this));

			_normal[vertexIndex[0] - 1] = tempNormals[normalIndex[0] - 1];
			_normal[vertexIndex[1] - 1] = tempNormals[normalIndex[1] - 1];
			_normal[vertexIndex[2] - 1] = tempNormals[normalIndex[2] - 1];

			_textCoord[vertexIndex[0] - 1] = tempTextures[textureIndex[0] - 1];
			_textCoord[vertexIndex[1] - 1] = tempTextures[textureIndex[1] - 1];
			_textCoord[vertexIndex[2] - 1] = tempTextures[textureIndex[2] - 1];
		}
	}

	fclose(file);

	if (incorrectPrimitives > 0)
	{
		std::cout << incorrectPrimitives << " primitives were not a triangle!" << std::endl;
	}

	return true;
}

/// [Face]

TriangleMesh::Face::Face(TriangleMesh* mesh) :
	_triangleMesh(mesh)
{
	_index[0] = _index[1] = _index[2] = 0;
}

TriangleMesh::Face::Face(const unsigned i1, const unsigned i2, const unsigned i3, TriangleMesh* mesh) :
	_triangleMesh(mesh)
{
	this->setIndexes(i1, i2, i3);
}

TriangleMesh::Face::Face(const Face& face) :
	_triangleMesh(face._triangleMesh)
{
	this->setIndexes(face._index[0], face._index[1], face._index[2]);
}

TriangleMesh::Face::~Face()
{
}

vec2 TriangleMesh::Face::getTextCoord(const vec3& minPoint) const
{
	// Christer Ericson's Real-Time Collision Detection:  Cramer's rule

	float u, v, w;

	const vec3 v0	= _triangle.getP2() - _triangle.getP1(), v1 = _triangle.getP3() - _triangle.getP1(), v2 = minPoint - _triangle.getP1();
	float d00		= glm::dot(v0, v0);
	float d01		= glm::dot(v0, v1);
	float d11		= glm::dot(v1, v1);
	float d20		= glm::dot(v2, v0);
	float d21		= glm::dot(v2, v1);
	float denom		= d00 * d11 - d01 * d01;
	v = (d11 * d20 - d01 * d21) / denom;
	w = (d00 * d21 - d01 * d20) / denom;
	u = 1.0f - v - w;
	
	return u * _triangleMesh->_textCoord[_index[0]] + v * _triangleMesh->_textCoord[_index[1]] + w * _triangleMesh->_textCoord[_index[2]];
}

TriangleMesh::Face& TriangleMesh::Face::operator=(const Face& face)
{
	this->setIndexes(face._index[0], face._index[1], face._index[2]);
	this->_triangleMesh = face._triangleMesh;

	return *this;
}

void TriangleMesh::Face::setIndexes(unsigned i1, unsigned i2, unsigned i3)
{
	_index[0] = i1;
	_index[1] = i2;
	_index[2] = i3;

	this->updateTriangle();
}

Triangle3D* TriangleMesh::Face::updateTriangle()
{
	_triangle = Triangle3D(_triangleMesh->_position[_index[0]], _triangleMesh->_position[_index[1]], _triangleMesh->_position[_index[2]]);

	return &_triangle;
}
