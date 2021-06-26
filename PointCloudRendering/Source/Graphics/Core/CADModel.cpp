#include "stdafx.h"
#include "CADModel.h"

#include <filesystem>
#include "Graphics/Application/MaterialList.h"
#include "Graphics/Core/ShaderList.h"
#include "Graphics/Core/VAO.h"
#include "Utilities/FileManagement.h"
#include "Utilities/ChronoUtilities.h"

// Initialization of static attributes
std::unordered_map<std::string, std::unique_ptr<Material>> CADModel::_cadMaterials;
std::unordered_map<std::string, std::unique_ptr<Texture>> CADModel::_cadTextures;

/// [Public methods]

CADModel::CADModel(const std::string& filename, const std::string& textureFolder, const bool useBinary) : 
	Model3D(mat4(1.0f), 1)
{
	_filename = filename;
	_textureFolder = textureFolder;
	_useBinary = useBinary;
}

CADModel::~CADModel()
{
}

bool CADModel::load(const mat4& modelMatrix)
{
	if (!_loaded)
	{
		bool success = false, binaryExists = false;

		if (_useBinary && (binaryExists = std::filesystem::exists(_filename + BINARY_EXTENSION)))
		{
			success = this->loadModelFromBinaryFile();
		}
			
		if (!success)
		{
			success = this->loadModelFromOBJ(modelMatrix);
		}

		if (!binaryExists && success)
		{
			this->writeToBinary();
		}

		return (_loaded = true);
	}

	return false;
}

/// [Protected methods]

void CADModel::computeMeshData(ModelComponent* modelComp)
{
	ComputeShader* shader = ShaderList::getInstance()->getComputeShader(RendEnum::MODEL_MESH_GENERATION);
	const int arraySize = modelComp->_topology.size();
	const int numGroups = ComputeShader::getNumGroups(arraySize);

	GLuint modelBufferID, meshBufferID, outBufferID;
	modelBufferID = ComputeShader::setReadBuffer(modelComp->_geometry);
	meshBufferID = ComputeShader::setReadBuffer(modelComp->_topology);
	outBufferID = ComputeShader::setWriteBuffer(GLuint(), arraySize * 4);

	shader->bindBuffers(std::vector<GLuint> { modelBufferID, meshBufferID, outBufferID });
	shader->use();
	shader->setUniform("size", arraySize);
	shader->setUniform("restartPrimitiveIndex", Model3D::RESTART_PRIMITIVE_INDEX);
	shader->execute(numGroups, 1, 1, ComputeShader::getMaxGroupSize(), 1, 1);

	FaceGPUData* faceData = shader->readData(meshBufferID, FaceGPUData());
	GLuint* rawMeshData = shader->readData(outBufferID, GLuint());
	modelComp->_topology = std::move(std::vector<FaceGPUData>(faceData, faceData + arraySize));
	modelComp->_triangleMesh = std::move(std::vector<GLuint>(rawMeshData, rawMeshData + arraySize * 4));

	glDeleteBuffers(1, &modelBufferID);
	glDeleteBuffers(1, &meshBufferID);
	glDeleteBuffers(1, &outBufferID);
}

Material* CADModel::createMaterial(ModelComponent* modelComp)
{
	static const std::string nullMaterialName = "None";

	Material* material = MaterialList::getInstance()->getMaterial(CGAppEnum::MATERIAL_CAD_WHITE);
	Model3D::ModelComponentDescription* modelDescription = &modelComp->_modelDescription;
	std::string name = std::string(modelDescription->_materialName);
	std::string mapKd = std::string(modelDescription->_mapKd);
	std::string mapKs = std::string(modelDescription->_mapKs);

	if (!name.empty() && name != nullMaterialName)
	{
		auto itMaterial = _cadMaterials.find(name);

		if (itMaterial == _cadMaterials.end())
		{
			material = new Material();
			Texture* kad, * ks;

			if (!mapKd.empty())
			{
				kad = new Texture(_textureFolder + mapKd);
			}
			else
			{
				kad = new Texture(vec4(modelDescription->_kd, 1.0f));
			}

			if (!mapKs.empty())
			{
				ks = new Texture(_textureFolder + mapKs);
			}
			else
			{
				ks = new Texture(vec4(modelDescription->_ks, 1.0f));
				material->setShininess(modelDescription->_ns);
			}

			material->setTexture(Texture::KAD_TEXTURE, kad);
			material->setTexture(Texture::KS_TEXTURE, ks);

			_cadMaterials[name] = std::unique_ptr<Material>(material);
			_cadTextures[name + "-kad"] = std::unique_ptr<Texture>(kad);
			_cadTextures[name + "-ks"] = std::unique_ptr<Texture>(ks);
		}
		else
		{
			material = itMaterial->second.get();
		}
	}

	return material;
}

void CADModel::createModelComponent(objl::Mesh* mesh, ModelComponent* modelComp)
{
	VertexGPUData vertexData;

	for (int j = 0; j < mesh->Vertices.size(); j++)
	{
		vertexData._position = vec3(mesh->Vertices[j].Position.X, mesh->Vertices[j].Position.Y, mesh->Vertices[j].Position.Z);
		vertexData._normal = vec3(mesh->Vertices[j].Normal.X, mesh->Vertices[j].Normal.Y, mesh->Vertices[j].Normal.Z);
		vertexData._textCoord = vec2(mesh->Vertices[j].TextureCoordinate.X, mesh->Vertices[j].TextureCoordinate.Y);

		modelComp->_geometry.push_back(vertexData);
	}

	for (int j = 0; j < mesh->Indices.size(); j += 3)
	{
		modelComp->_topology.push_back(Model3D::FaceGPUData());
		modelComp->_topology[j / 3]._vertices = uvec3(mesh->Indices[j], mesh->Indices[j + 1], mesh->Indices[j + 2]);
		modelComp->_topology[j / 3]._modelCompID = modelComp->_id;
	}

	if (!modelComp->_material)
	{
		modelComp->_modelDescription = Model3D::ModelComponentDescription(mesh);
		modelComp->_material = this->createMaterial(modelComp);
		modelComp->setName(modelComp->_modelDescription._modelName);
	}
}

void CADModel::generateGeometryTopology(Model3D::ModelComponent* modelComp, const mat4& modelMatrix)
{
	ComputeShader* shader = ShaderList::getInstance()->getComputeShader(RendEnum::MODEL_APPLY_MODEL_MATRIX);
	const int arraySize = modelComp->_geometry.size();
	const int numGroups = ComputeShader::getNumGroups(arraySize);

	GLuint modelBufferID;
	modelBufferID = ComputeShader::setReadBuffer(modelComp->_geometry);

	shader->bindBuffers(std::vector<GLuint> { modelBufferID});
	shader->use();
	shader->setUniform("mModel", modelMatrix * _modelMatrix);
	shader->setUniform("size", arraySize);
	if (modelComp->_material) modelComp->_material->applyMaterial4ComputeShader(shader);
	shader->execute(numGroups, 1, 1, ComputeShader::getMaxGroupSize(), 1, 1);

	VertexGPUData* data = shader->readData(modelBufferID, VertexGPUData());
	modelComp->_geometry = std::move(std::vector<VertexGPUData>(data, data + arraySize));

	this->computeTangents(modelComp);
	this->computeMeshData(modelComp);

	glDeleteBuffers(1, &modelBufferID);

	// Wireframe & point cloud are derived from previous operations
	modelComp->buildPointCloudTopology();
	modelComp->buildWireframeTopology();
}

std::string CADModel::getKeyValue(std::map<std::string, std::string>& keyMap, std::string& modelName, std::string& defaultClass)
{
	auto itFirst = keyMap.begin();
	unsigned maxMatches = 0, lastMatches;
	std::string currentClass = defaultClass;

	while (itFirst != keyMap.end())
	{
		auto itString = std::string(modelName).find(itFirst->first);

		if (itString != std::string::npos)
		{
			lastMatches = itFirst->first.size();

			if (lastMatches >= maxMatches)
			{
				maxMatches = lastMatches;
				currentClass = itFirst->second;
			}
		}

		++itFirst;
	}

	return currentClass;
}

bool CADModel::loadModelFromBinaryFile()
{
	bool success;

	if (success = this->readBinary(_filename +  BINARY_EXTENSION, _modelComp))
	{
		for (ModelComponent* modelComp : _modelComp)
		{
			modelComp->_material = this->createMaterial(modelComp);
			modelComp->setName(modelComp->_modelDescription._modelName);
			this->setVAOData(modelComp);
		}
	}

	return success;
}

bool CADModel::loadModelFromOBJ(const mat4& modelMatrix)
{
	objl::Loader loader;
	bool success = loader.LoadFile(_filename + OBJ_EXTENSION);
	unsigned int numNameCollisions = 0;

	if (success)
	{
		std::string modelName, currentModelName;

		if (loader.LoadedMeshes.size())
		{
			unsigned index = 0;

			while (index < loader.LoadedMeshes.size() && modelName.empty())
			{
				modelName = loader.LoadedMeshes[0].MeshName;
				++index;
			}
		}

		while (_modelComp.size() < loader.LoadedMeshes.size())
		{
			_modelComp.push_back(new ModelComponent(this));
		}

		for (int i = 0; i < loader.LoadedMeshes.size(); i++)
		{
			this->createModelComponent(&(loader.LoadedMeshes[i]), _modelComp[i]);
			this->generateGeometryTopology(_modelComp[i], modelMatrix);
			this->setVAOData(_modelComp[i]);

			currentModelName = std::string(_modelComp[i]->_modelDescription._modelName);
			numNameCollisions += currentModelName == modelName;
		}

		if (_modelComp.size() > 1 && numNameCollisions > std::ceil(loader.LoadedMeshes.size() / 5))
		{
			for (ModelComponent* modelComp : _modelComp)
			{
				strcpy(modelComp->_modelDescription._modelName, modelComp->_modelDescription._materialName);
			}
		}
	}

	return success;
}

bool CADModel::readBinary(const std::string& filename, const std::vector<Model3D::ModelComponent*>& modelComp)
{
	std::ifstream fin(filename, std::ios::in | std::ios::binary);
	if (!fin.is_open())
	{
		return false;
	}

	size_t numModelComps, numVertices, numTriangles, numIndices;

	fin.read((char*)&numModelComps, sizeof(size_t));
	while (_modelComp.size() < numModelComps)
	{
		_modelComp.push_back(new ModelComponent(this));
	}

	for (Model3D::ModelComponent* model : modelComp)
	{
		fin.read((char*)&numVertices, sizeof(size_t));
		model->_geometry.resize(numVertices);
		fin.read((char*)&model->_geometry[0], numVertices * sizeof(Model3D::VertexGPUData));

		fin.read((char*)&numTriangles, sizeof(size_t));
		model->_topology.resize(numTriangles);
		fin.read((char*)&model->_topology[0], numTriangles * sizeof(Model3D::FaceGPUData));

		fin.read((char*)&numIndices, sizeof(size_t));
		model->_triangleMesh.resize(numIndices);
		fin.read((char*)&model->_triangleMesh[0], numIndices * sizeof(GLuint));

		fin.read((char*)&numIndices, sizeof(size_t));
		model->_pointCloud.resize(numIndices);
		fin.read((char*)&model->_pointCloud[0], numIndices * sizeof(GLuint));

		fin.read((char*)&numIndices, sizeof(size_t));
		model->_wireframe.resize(numIndices);
		fin.read((char*)&model->_wireframe[0], numIndices * sizeof(GLuint));

		fin.read((char*)&model->_modelDescription, sizeof(Model3D::ModelComponentDescription));
	}

	fin.close();

	return true;
}

void CADModel::readClassFile(const std::string& filename, std::map<std::string, std::string>& keyMap, std::string& defaultClass)
{
	const static char KEYWORD_DELIMITER = ';';

	// File management
	size_t delimiterIndex = std::string::npos;
	std::string currentLine, classKey, keyword, leftLine;
	std::list<std::string> keywords;
	std::stringstream line;
	std::ifstream inputStream;

	inputStream.open(filename.c_str());

	if (inputStream.fail()) return;

	while (!(inputStream >> std::ws).eof())
	{
		classKey = keyword = leftLine = "";
		keywords.clear();

		std::getline(inputStream, currentLine);

		if (currentLine.find(COMMENT_CHAR) != 0 && !currentLine.empty())		// Comment line
		{
			line.clear();
			line.str(currentLine);
			std::getline(line, classKey, '\t');

			if (classKey.empty())					// In case no tab alignment was followed
			{
				std::getline(line, classKey, ' ');
			}

			line >> leftLine;

			while (!leftLine.empty())
			{
				delimiterIndex = leftLine.find_first_of(KEYWORD_DELIMITER);

				if (delimiterIndex != std::string::npos)
				{
					keyword = leftLine.substr(0, delimiterIndex);
					leftLine = leftLine.substr(delimiterIndex + 1, leftLine.length() - delimiterIndex - 1);
				}
				else
				{
					keyword = leftLine;
					leftLine.clear();
				}

				if (!keyword.empty())
				{
					keywords.push_back(keyword);
				}
			}

			if (!classKey.empty() && keywords.empty())
			{
				defaultClass = classKey;
			}
			else
			{
				for (std::string& keywordString : keywords)
				{
					keyMap[keywordString] = classKey;
				}
			}
		}
	}

	inputStream.close();
}

void CADModel::setVAOData(ModelComponent* modelComp)
{
	VAO* vao = new VAO(true);

	modelComp->_topologyIndicesLength[RendEnum::IBO_POINT_CLOUD] = modelComp->_pointCloud.size();
	modelComp->_topologyIndicesLength[RendEnum::IBO_WIREFRAME] = modelComp->_wireframe.size();
	modelComp->_topologyIndicesLength[RendEnum::IBO_TRIANGLE_MESH] = modelComp->_triangleMesh.size();

	vao->setVBOData(modelComp->_geometry);
	vao->setIBOData(RendEnum::IBO_POINT_CLOUD, modelComp->_pointCloud);
	vao->setIBOData(RendEnum::IBO_WIREFRAME, modelComp->_wireframe);
	vao->setIBOData(RendEnum::IBO_TRIANGLE_MESH, modelComp->_triangleMesh);

	modelComp->_vao = vao;
}

bool CADModel::writeToBinary()
{
	std::ofstream fout(_filename + ".bin", std::ios::out | std::ios::binary);
	if (!fout.is_open())
	{
		return false;
	}

	size_t numIndices;
	const size_t numModelComps = _modelComp.size();
	fout.write((char*)&numModelComps, sizeof(size_t));

	for (Model3D::ModelComponent* model : _modelComp)
	{
		const size_t numVertices = model->_geometry.size();
		fout.write((char*)&numVertices, sizeof(size_t));
		fout.write((char*)&model->_geometry[0], numVertices * sizeof(Model3D::VertexGPUData));

		const size_t numTriangles = model->_topology.size();
		fout.write((char*)&numTriangles, sizeof(size_t));
		fout.write((char*)&model->_topology[0], numTriangles * sizeof(Model3D::FaceGPUData));

		numIndices = model->_triangleMesh.size();
		fout.write((char*)&numIndices, sizeof(size_t));
		fout.write((char*)&model->_triangleMesh[0], numIndices * sizeof(GLuint));

		numIndices = model->_pointCloud.size();
		fout.write((char*)&numIndices, sizeof(size_t));
		fout.write((char*)&model->_pointCloud[0], numIndices * sizeof(GLuint));

		numIndices = model->_wireframe.size();
		fout.write((char*)&numIndices, sizeof(size_t));
		fout.write((char*)&model->_wireframe[0], numIndices * sizeof(GLuint));

		fout.write((char*)&model->_modelDescription, sizeof(Model3D::ModelComponentDescription));
	}

	fout.close();

	return true;
}