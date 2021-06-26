#pragma once

#include "Geometry/3D/AABB.h"
#include "Geometry/3D/Plane.h"
#include "Geometry/3D/Ray3D.h"
#include "Geometry/3D/Triangle3D.h"

/**
*	@file Triangle3D.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 08/06/2019
*/

/**
*	@brief Set of connected triangles (topology).
*/
class TriangleMesh
{
	friend class Model3D;
	friend class ModelOBJ;
	friend class PlanarSurface;
	friend class Terrain;

public:
	class Face;

protected:
	// [Per vertex information]	
	std::vector<vec4>	_position;								//!< Vertices 
	std::vector<vec3>	_normal;								//!< Normals of vertices
	std::vector<vec3>	_tangent;								//!< Computed data
	std::vector<vec2>	_textCoord;								//!< Texture coordinates of vertices

	// [Topology]
	std::vector<Face>	_face;									//!< Mesh faces

	// [Spatial data]
	AABB				_aabb;									//!< Axis-aligned bounding box

protected:
	/**
	*	@brief Auxiliar method for copy constructor and assignment operator.
	*/
	void copyAttributes(const TriangleMesh& mesh);

	/**
	*	@brief Reads an obj file to load its data into a triangle mesh.
	*/
	bool loadOBJ(const std::string& filename);

public:
	/**
	*	@brief Constructor of a triangle mesh from an obj file.
	*/
	TriangleMesh(const std::string& objFilename);

	/**
	*	@brief Empty triangle mesh.
	*/
	TriangleMesh();

	/**
	*	@brief Copy constructor.
	*/
	TriangleMesh(const TriangleMesh& mesh);

	/**
	*	@brief Destructor.
	*/
	virtual ~TriangleMesh();

	/**
	*	@return Bounding box.
	*/
	AABB aabb() const { return _aabb; }

	/**
	*	@brief Classifies the plane taking into account its relative position to the triangle mesh.
	*/
	void classify(Plane& plane);

	/**
	*	@brief Computes tangents for every point of the triangle mesh, since it's never included by default.
	*/
	void computeTangents();

	/**
	*	@return Number of faces of the mesh.
	*/
	size_t getNumTriangles() const { return _face.size(); }

	/**
	*	@return Point from the mesh.
	*/
	vec3 getVertex(int i) const;

	/**
	*	@return Number of vertices.
	*/
	size_t getNumVertices()  const { return _position.size(); }

	/**
	*	@brief Assignment operator overriding.
	*/
	TriangleMesh& operator=(const TriangleMesh& mesh);

	/**
	*	@brief Checks if a point is inside the triangle mesh with an exhaustive method.
	*/
	bool pointInMesh(const vec3& point);

	/**
	*	@brief Adds a new face to the triangle mesh.
	*	@return Triangle which has just been added.
	*/
	Triangle3D* pushBackFace(const unsigned i1, const unsigned i2, const unsigned i3);

	/**
	*	@brief Includes a new vertex (and its related information) at the triangle mesh.
	*	@return Index which occupies the new vertex.
	*/
	size_t pushBackVertex(const vec3& position, const vec3& normal, const vec2& textCoord = vec2(1.0f), const vec3& tangent = vec3(1.0f));

	/**
	*	@brief Calculates (exhaustively) all the triangles the given ray intersects.
	*/
	bool rayTraversalExh(Ray3D& ray, std::vector<vec3>& point, std::vector<Triangle3D>& triangle);
};

class TriangleMesh::Face 
{
public:
	enum FacePlaneRelation
	{
		NO_RELATION, POSITIVE, NEGATIVE, POINT_INT, SEGMENT_INT, COPLANAR
	};

public:
	unsigned		_index[3];						//!< Indexes of vertices
	Triangle3D		_triangle;						//!< Core
	TriangleMesh*	_triangleMesh;					//!< Mesh where those vertices belong to

public:
	/**
	*	@brief Default constructor.
	*	@param mesh Mesh from where we can retrieve the vertices
	*/
	Face(TriangleMesh* mesh);

	/**
	*	@brief Parametrized constructor.
	*	@param i1, i2, i3 Indexes of vertices.
	*	@param mesh Mesh from where we can retrieve the vertices
	*/
	Face(const unsigned i1, const unsigned i2, const unsigned i3, TriangleMesh* mesh);

	/**
	*	@brief Copy constructor.
	*/
	Face(const Face& face);

	/**
	*	@brief Destructor.
	*/
	virtual ~Face();

	/**
	*	@return Triangle normal.
	*/
	vec3 getNormal() const { return _triangle.normal(); }

	/**
	*	@return Triangle tangent, even though we return the tangent of any vertex from the triangle.
	*/
	vec3 getTangent() const { return _triangleMesh->_tangent[_index[0]]; }

	/**
	*	@return Texture coordinate for two parametric values which tells us the position inside the triangle.
	*/
	vec2 getTextCoord(const vec3& minPoint) const;

	/**
	*	@brief Returns a vertex index.
	*/
	int getVertexIndex(int i) { return _index[i]; }

	/**
	*	@brief Assignment operator overriding.
	*/
	Face& operator=(const Face& face);

	/**
	*	@brief Modifies the indexes of vertices.
	*	@param i1, i2, i3 Indexes of vertices.
	*/
	void setIndexes(unsigned i1, unsigned i2, unsigned i3);

	/**
	*	@brief Updates the triangle data as the vertices could have changed.
	*	@return Updated triangle.
	*/
	Triangle3D* updateTriangle();
};

