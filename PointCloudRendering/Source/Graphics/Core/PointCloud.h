#pragma once

#include "Geometry/3D/AABB.h"
#include "Graphics/Application/RenderingParameters.h"
#include "Graphics/Core/Model3D.h"

/**
*	@file Pix4DPointCloud.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 09/24/2020
*/

/**
*	@brief 
*/
class PointCloud : public Model3D
{
public:
	struct PointModel
	{
		vec4	_point;
		vec4	_rgb;
	};

protected:
	const static std::string	WRITE_POINT_CLOUD_FOLDER;			//!<

protected:
	std::string					_filename;									//!<
	bool						_useBinary;									//!<

	// Spatial information
	AABB						_aabb;										//!<
	std::vector<PointModel>		_points;									//!<			

protected:
	/**
	*	@brief Computes a triangle mesh buffer composed only by indices.
	*/
	void computeCloudData();

	/**
	*	@brief Fills the content of model component with binary file data.
	*/
	bool loadModelFromBinaryFile();

	/**
	*	@brief Generates geometry via GPU.
	*/
	bool loadModelFromPLY(const mat4& modelMatrix);

	/**
	*	@brief Loads the PLY point cloud from a binary file, if possible.
	*/
	virtual bool readBinary(const std::string& filename, const std::vector<Model3D::ModelComponent*>& modelComp);

	/**
	*	@brief Communicates the model structure to GPU for rendering purposes.
	*/
	virtual void setVAOData();

	/**
	*	@brief
	*/
	void threadedWritePointCloud(const std::string& filename, const bool ascii);

	/**
	*	@brief Writes the model to a binary file in order to fasten the following executions.
	*	@return Success of writing process.
	*/
	virtual bool writeToBinary(const std::string& filename);

public:
	/**
	*	@brief 
	*/
	PointCloud(const std::string& filename, const bool useBinary, const mat4& modelMatrix = mat4(1.0f));

	/**
	*	@brief
	*/
	virtual ~PointCloud();

	/**
	*	@brief Loads the point cloud, either from a binary or a PLY file.
	*	@param modelMatrix Model transformation matrix.
	*	@return True if the point cloud could be properly loaded.
	*/
	virtual bool load(const mat4& modelMatrix = mat4(1.0f));

	/**
	*	@brief Updates the current Axis-Aligned Bounding-Box.
	*/
	void updateBoundaries(const vec3& xyz) { _aabb.update(xyz); }

	/**
	*	@brief Writes point cloud as a PLY file.
	*/
	bool writePointCloud(const std::string& filename, const bool ascii);

	// Getters

	/**
	*	@return
	*/
	AABB getAABB() { return _aabb; }

	/**
	*	@return Path where the point cloud is saved.
	*/
	std::string getFilename() { return _filename; }

	/**
	*	@brief
	*/
	unsigned getNumberOfPoints() { return unsigned(_points.size()); }

	/**
	*	@return
	*/
	std::vector<PointModel>* getPoints() { return &_points; }
};

