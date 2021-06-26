#include "stdafx.h"
#include "ForestEditorNode.h"

#include <bitset>

/// [Static methods]

const char* ForestEditorNode::_forestNodeName[] = { "Scene Root", "Simplex Noise", "Noise Simple", "Terrain", "Forest", "Grass", "Water", "Watcher", "Transmission Tower" };

ForestEditorNode::NodeImagePathMap	ForestEditorNode::_nodeImagePath;
ForestEditorNode::NodeApplicatorMap ForestEditorNode::_nodeApplicator;
ForestEditorNode::TextureMap		ForestEditorNode::_nodeImage;
ForestEditorNode::SceneDescription	ForestEditorNode::_sceneDescription;

const uvec2		ForestEditorNode::MAX_TERRAIN_SUBDIVISIONS = uvec2(1000, 1000);
const uvec2		ForestEditorNode::MIN_TERRAIN_SUBDIVISIONS = uvec2(100, 100);
const vec2		ForestEditorNode::MAX_TERRAIN_SIZE = vec2(300.0f, 300.0f);
const vec2		ForestEditorNode::MIN_TERRAIN_SIZE = vec2(5.0f, 5.0f);
const uvec2		ForestEditorNode::MAX_WATER_SUBDIVISIONS = uvec2(500, 500);
const uvec2		ForestEditorNode::MIN_WATER_SUBDIVISIONS = uvec2(100, 100);
const GLuint	ForestEditorNode::MAX_TREES = 800;
const GLuint	ForestEditorNode::MIN_TREES = 0;
const GLuint	ForestEditorNode::MAX_GRASS_SEEDS = 3e6;
const GLuint	ForestEditorNode::MIN_GRASS_SEEDS = 0;
const float		ForestEditorNode::MAX_NOISE_FREQUENCY = 0.1f;
const float		ForestEditorNode::MIN_NOISE_FREQUENCY = 0.00001f;

/// [Public methods]

ForestEditorNode::ForestEditorNode(ForestNodeType type) : _nodeType(type)
{
	_numTerrainSubdivisions = TerrainParameters::TERRAIN_SUBDIVISIONS;
	_numWaterSubdivisions	= TerrainParameters::WATER_SUBDIVISIONS;
	_numGrassSeeds			= TerrainParameters::NUM_GRASS_SEEDS;
	_terrainSize			= TerrainParameters::TERRAIN_SIZE;
	_noiseFrequency			= TerrainParameters::FREQUENCY;
	_enableModel			= true;

	_sceneDescription._enableTransmissionTower	= true;
	_sceneDescription._enableWatcher			= true;
	_sceneDescription._noiseFrequency			= _noiseFrequency;
	_sceneDescription._numWaterSubdivisions		= _numWaterSubdivisions;
	_sceneDescription._numTerrainSubdivisions	= _numTerrainSubdivisions;
	_sceneDescription._numGrassSeeds			= _numGrassSeeds;
	_sceneDescription._terrainSize				= _terrainSize;

	for (int modelIdx = 0; modelIdx < TerrainParameters::TREES_MODEL.size(); ++modelIdx)
	{
		_numTrees[modelIdx] = TerrainParameters::TREES_MODEL[modelIdx];
		_sceneDescription._numTrees[modelIdx] = _numTrees[modelIdx];
	}
}

ForestEditorNode::~ForestEditorNode()
{
}

void ForestEditorNode::drawNode()
{
	_nodeApplicator[_nodeType]->drawNode(this);
}

void ForestEditorNode::updateLinks(LinkMap& links)
{
	_nodeApplicator[_nodeType]->updateLinks(links);
}

void ForestEditorNode::initializeTextures()
{
	ForestEditorNode::buildImagePathMap();
	ForestEditorNode::buildApplicatorMap();
	ForestEditorNode::loadTextures();
}

bool ForestEditorNode::saveDescriptionToBinary()
{
	std::ofstream fout(TerrainParameters::BINARY_LOCATION, std::ios::out | std::ios::binary);
	if (!fout.is_open()) return false;

	fout.write((char*)&_sceneDescription, sizeof(SceneDescription));
	fout.close();

	return true;
}

/// [Protected methods]

void ForestEditorNode::buildApplicatorMap()
{
	_nodeApplicator = ForestEditorNode::NodeApplicatorMap(ForestNodeType::NUM_NODE_TYPES);

	_nodeApplicator[ForestNodeType::SCENE_ROOT].reset(new SceneRootNodeApplicator);
	_nodeApplicator[ForestNodeType::NOISE_SIMPLEX].reset(new SimplexNodeApplicator);
	_nodeApplicator[ForestNodeType::NOISE_RAW].reset(new SimpleNoiseApplicator);
	_nodeApplicator[ForestNodeType::TERRAIN_SURFACE].reset(new TerrainNodeApplicator);
	_nodeApplicator[ForestNodeType::WATER].reset(new WaterNodeApplicator);
	_nodeApplicator[ForestNodeType::FOREST].reset(new ForestNodeApplicator);
	_nodeApplicator[ForestNodeType::GRASS].reset(new GrassNodeApplicator);
	_nodeApplicator[ForestNodeType::WATCHER_MODEL].reset(new WatcherNodeApplicator);
	_nodeApplicator[ForestNodeType::TRANSMISSION_TOWER_MODEL].reset(new TransmissionTowerNodeApplicator);
}

void ForestEditorNode::buildImagePathMap()
{
	_nodeImagePath = ForestEditorNode::NodeImagePathMap(ForestNodeType::NUM_NODE_TYPES);

	_nodeImagePath[ForestNodeType::SCENE_ROOT]					= "";
	_nodeImagePath[ForestNodeType::NOISE_SIMPLEX]				= "Assets/Images/SimplexTerrain-LowRes.png";
	_nodeImagePath[ForestNodeType::NOISE_RAW]					= "Assets/Images/SimplexTerrain-LowRes.png";
	_nodeImagePath[ForestNodeType::TERRAIN_SURFACE]				= "Assets/Images/Terrain-LowRes.png";
	_nodeImagePath[ForestNodeType::WATER]						= "Assets/Images/SimplexTerrain-LowRes.png";
	_nodeImagePath[ForestNodeType::FOREST]						= "Assets/Images/Trees-LowRes.png";
	_nodeImagePath[ForestNodeType::GRASS]						= "Assets/Images/Grass-LowRes.png";
	_nodeImagePath[ForestNodeType::WATCHER_MODEL]				= "Assets/Images/Watcher-LowRes.png";
	_nodeImagePath[ForestNodeType::TRANSMISSION_TOWER_MODEL]	= "Assets/Images/TransmissionTower-LowRes.png";
}

void ForestEditorNode::loadTextures()
{
	_nodeImage = ForestEditorNode::TextureMap(ForestNodeType::NUM_NODE_TYPES);
	
	for (int nodeIdx = 0; nodeIdx < NUM_NODE_TYPES; ++nodeIdx)
	{
		if (!_nodeImagePath[nodeIdx].empty())
		{
			Image image(ForestEditorNode::_nodeImagePath[nodeIdx]);
			image.flipImageVertically();
			_nodeImage[nodeIdx].reset(new Texture(&image, GL_CLAMP, GL_CLAMP, GL_LINEAR, GL_LINEAR));
		}
	}
}

/// [Forest Node Applicator]

// Base class

int ForestEditorNode::ForestEditorNodeApplicator::getAttributeId(const unsigned nodeType, const unsigned pinType, const bool output)
{
	int nodeId = nodeType * NUM_NODE_TYPES * 2;
	nodeId += pinType + NUM_NODE_TYPES * int(output);

	return nodeId;
}

void ForestEditorNode::ForestEditorNodeApplicator::leaveSpace(const unsigned space)
{
	for (int i = 0; i < space; ++i)
	{
		ImGui::Spacing();
	}
}

// Root

void ForestEditorNode::SceneRootNodeApplicator::drawNode(ForestEditorNode* node)
{
	imnodes::PushColorStyle(imnodes::ColorStyle_TitleBar, IM_COL32(210, 0, 0, 255));
	imnodes::PushColorStyle(imnodes::ColorStyle_TitleBarSelected, IM_COL32(255, 0, 0, 255));
	imnodes::PushColorStyle(imnodes::ColorStyle_TitleBarHovered, IM_COL32(255, 0, 0, 255));
	
	imnodes::BeginNode(node->_nodeType);
	
	imnodes::BeginNodeTitleBar();
	ImGui::TextUnformatted(ForestEditorNode::_forestNodeName[node->_nodeType]);
	imnodes::EndNodeTitleBar();

	{
		imnodes::BeginOutputAttribute(getAttributeId(node->_nodeType, TERRAIN_SURFACE, true));
		ImGui::Text("Terrain");
		imnodes::EndOutputAttribute();

		imnodes::BeginOutputAttribute(getAttributeId(node->_nodeType, WATER, true));
		ImGui::Text("Water");
		imnodes::EndOutputAttribute();

		imnodes::BeginOutputAttribute(getAttributeId(node->_nodeType, FOREST, true));
		ImGui::Text("Forest");
		imnodes::EndOutputAttribute();

		imnodes::BeginOutputAttribute(getAttributeId(node->_nodeType, GRASS, true));
		ImGui::Text("Grass");
		imnodes::EndOutputAttribute();

		imnodes::BeginOutputAttribute(getAttributeId(node->_nodeType, WATCHER_MODEL, true));
		ImGui::Text("Watcher");
		imnodes::EndOutputAttribute();

		imnodes::BeginOutputAttribute(getAttributeId(node->_nodeType, TRANSMISSION_TOWER_MODEL, true));
		ImGui::Text("Transmission Tower");
		imnodes::EndOutputAttribute();

	}

	imnodes::EndNode();

	imnodes::PopColorStyle();
	imnodes::PopColorStyle();
	imnodes::PopColorStyle();
}

void ForestEditorNode::SceneRootNodeApplicator::updateLinks(LinkMap& links)
{
	int outputPin, outputPinCopy, startPin, startPinCopy;
	
	for (int outputNodeIdx = TERRAIN_SURFACE; outputNodeIdx < NUM_NODE_TYPES; ++outputNodeIdx)
	{
		outputPin = outputPinCopy = getAttributeId(SCENE_ROOT, outputNodeIdx, true);
		startPin = startPinCopy = getAttributeId(outputNodeIdx, SCENE_ROOT, false);
		
		if (imnodes::IsLinkCreated(&outputPin, &startPin) && (outputPin == outputPinCopy && startPin == startPinCopy))
		{
			links.push_back(std::make_pair(outputPin, startPin));
		}
	}
}

// Simplex Noise

void ForestEditorNode::SimplexNodeApplicator::drawNode(ForestEditorNode* node)
{
	imnodes::BeginNode(node->_nodeType);

	imnodes::BeginNodeTitleBar();
	ImGui::TextUnformatted(ForestEditorNode::_forestNodeName[node->_nodeType]);
	imnodes::EndNodeTitleBar();

	ImGui::Image((void*)_nodeImage[node->_nodeType]->getID(), ImVec2(150, 83));

	ImGui::PushItemWidth(80.0f);
	{
		this->leaveSpace(1);

		imnodes::BeginStaticAttribute(0);

		ImGui::SliderScalar("Frequency", ImGuiDataType_Float, &node->_noiseFrequency, &MIN_NOISE_FREQUENCY, &MAX_NOISE_FREQUENCY);

		imnodes::EndStaticAttribute();

		imnodes::BeginInputAttribute(getAttributeId(node->_nodeType, TERRAIN_SURFACE, false));
		imnodes::EndInputAttribute();
	}

	imnodes::EndNode();
}

// Simple Noise

void ForestEditorNode::SimpleNoiseApplicator::drawNode(ForestEditorNode* node)
{
	imnodes::BeginNode(node->_nodeType);

	imnodes::BeginNodeTitleBar();
	ImGui::TextUnformatted(ForestEditorNode::_forestNodeName[node->_nodeType]);
	imnodes::EndNodeTitleBar();

	ImGui::Image((void*)_nodeImage[node->_nodeType]->getID(), ImVec2(150, 83));

	ImGui::PushItemWidth(80.0f);
	{
		this->leaveSpace(1);

		imnodes::BeginStaticAttribute(0);
		
		ImGui::SliderScalar("Frequency", ImGuiDataType_Float, &node->_noiseFrequency, &MIN_NOISE_FREQUENCY, &MAX_NOISE_FREQUENCY);

		imnodes::EndStaticAttribute();

		imnodes::BeginInputAttribute(getAttributeId(node->_nodeType, TERRAIN_SURFACE, false));
		imnodes::EndInputAttribute();
	}

	imnodes::EndNode();
}

// Terrain

void ForestEditorNode::TerrainNodeApplicator::drawNode(ForestEditorNode* node)
{
	imnodes::BeginNode(node->_nodeType);

	imnodes::BeginNodeTitleBar();
	ImGui::TextUnformatted(ForestEditorNode::_forestNodeName[node->_nodeType]);
	imnodes::EndNodeTitleBar();

	ImGui::Image((void*)_nodeImage[node->_nodeType]->getID(), ImVec2(200, 84));

	ImGui::PushItemWidth(80.0f);
	{
		this->leaveSpace(1);

		imnodes::BeginStaticAttribute(0);

		ImGui::SliderScalar("Horizontal Subdiv.", ImGuiDataType_U16, &node->_numTerrainSubdivisions.x, &MIN_TERRAIN_SUBDIVISIONS.x, &MAX_TERRAIN_SUBDIVISIONS.x);
		ImGui::SliderScalar("Vertical Subdiv.", ImGuiDataType_U16, &node->_numTerrainSubdivisions.y, &MIN_TERRAIN_SUBDIVISIONS.y, &MAX_TERRAIN_SUBDIVISIONS.y);

		imnodes::EndStaticAttribute();

		imnodes::BeginInputAttribute(getAttributeId(node->_nodeType, SCENE_ROOT, false));
		imnodes::EndInputAttribute();

		imnodes::BeginOutputAttribute(getAttributeId(node->_nodeType, NOISE_SIMPLEX, true));
		
		ImGui::Text("Noise");
		
		imnodes::EndOutputAttribute();
	}

	imnodes::EndNode();
}

void ForestEditorNode::TerrainNodeApplicator::updateLinks(LinkMap& links)
{
	int outputPin, outputPinCopy, startPin, startPinCopy;

	for (int outputNodeIdx = NOISE_SIMPLEX; outputNodeIdx < TERRAIN_SURFACE; ++outputNodeIdx)
	{
		outputPin = outputPinCopy = getAttributeId(TERRAIN_SURFACE, outputNodeIdx, true);
		startPin = startPinCopy = getAttributeId(outputNodeIdx, TERRAIN_SURFACE, false);

		if (imnodes::IsLinkCreated(&outputPin, &startPin) && (outputPin == outputPinCopy && startPin == startPinCopy))
		{
			links.push_back(std::make_pair(outputPin, startPin));
		}
	}
}

// Water

void ForestEditorNode::WaterNodeApplicator::drawNode(ForestEditorNode* node)
{
	imnodes::BeginNode(node->_nodeType);

	imnodes::BeginNodeTitleBar();
	ImGui::TextUnformatted(ForestEditorNode::_forestNodeName[node->_nodeType]);
	imnodes::EndNodeTitleBar();

	ImGui::Image((void*)_nodeImage[node->_nodeType]->getID(), ImVec2(200, 110));

	ImGui::PushItemWidth(80.0f);
	{
		this->leaveSpace(1);

		imnodes::BeginStaticAttribute(0);

		ImGui::SliderScalar("Horizontal Subdiv.", ImGuiDataType_U16, &node->_numWaterSubdivisions.x, &MIN_WATER_SUBDIVISIONS.x, &MAX_WATER_SUBDIVISIONS.x);
		ImGui::SliderScalar("Vertical Subdiv.", ImGuiDataType_U16, &node->_numWaterSubdivisions.y, &MIN_WATER_SUBDIVISIONS.y, &MAX_WATER_SUBDIVISIONS.y);

		imnodes::EndStaticAttribute();

		imnodes::BeginInputAttribute(getAttributeId(node->_nodeType, SCENE_ROOT, false));
		imnodes::EndInputAttribute();
	}

	imnodes::EndNode();
}

// Forest

void ForestEditorNode::ForestNodeApplicator::drawNode(ForestEditorNode* node)
{
	imnodes::BeginNode(node->_nodeType);

	imnodes::BeginNodeTitleBar();
	ImGui::TextUnformatted(ForestEditorNode::_forestNodeName[node->_nodeType]);
	imnodes::EndNodeTitleBar();

	ImGui::Image((void*)_nodeImage[node->_nodeType]->getID(), ImVec2(160, 154));

	ImGui::PushItemWidth(80.0f);
	{
		this->leaveSpace(1);

		imnodes::BeginStaticAttribute(0);

		ImGui::SliderScalar("Num. trees (1)", ImGuiDataType_U16, &node->_numTrees[0], &MIN_TREES, &MAX_TREES);
		ImGui::SliderScalar("Num. trees (2)", ImGuiDataType_U16, &node->_numTrees[1], &MIN_TREES, &MAX_TREES);
		ImGui::SliderScalar("Num. trees (3)", ImGuiDataType_U16, &node->_numTrees[2], &MIN_TREES, &MAX_TREES);

		imnodes::EndStaticAttribute();

		imnodes::BeginInputAttribute(getAttributeId(node->_nodeType, SCENE_ROOT, false));
		imnodes::EndInputAttribute();
	}

	imnodes::EndNode();
}

// Grass

void ForestEditorNode::GrassNodeApplicator::drawNode(ForestEditorNode* node)
{
	imnodes::BeginNode(node->_nodeType);

	imnodes::BeginNodeTitleBar();
	ImGui::TextUnformatted(ForestEditorNode::_forestNodeName[node->_nodeType]);
	imnodes::EndNodeTitleBar();

	ImGui::Image((void*)_nodeImage[node->_nodeType]->getID(), ImVec2(160, 100));

	ImGui::PushItemWidth(80.0f);
	{
		this->leaveSpace(1);

		imnodes::BeginStaticAttribute(0);

		ImGui::SliderScalar("Num. Seeds", ImGuiDataType_U16, &node->_numGrassSeeds, &MIN_GRASS_SEEDS, &MAX_GRASS_SEEDS);

		imnodes::EndStaticAttribute();

		imnodes::BeginInputAttribute(getAttributeId(node->_nodeType, SCENE_ROOT, false));
		imnodes::EndInputAttribute();
	}

	imnodes::EndNode();
}

// Watcher

void ForestEditorNode::WatcherNodeApplicator::drawNode(ForestEditorNode* node)
{
	imnodes::BeginNode(node->_nodeType);

	imnodes::BeginNodeTitleBar();
	ImGui::TextUnformatted(ForestEditorNode::_forestNodeName[node->_nodeType]);
	imnodes::EndNodeTitleBar();

	ImGui::Image((void*)_nodeImage[node->_nodeType]->getID(), ImVec2(120, 140));

	ImGui::PushItemWidth(80.0f);
	{
		this->leaveSpace(1);

		imnodes::BeginStaticAttribute(0);

		ImGui::Checkbox("Enable Instancing", &node->_enableModel);

		imnodes::EndStaticAttribute();

		imnodes::BeginInputAttribute(getAttributeId(node->_nodeType, SCENE_ROOT, false));
		imnodes::EndInputAttribute();
	}

	imnodes::EndNode();
}

// Transmission tower

void ForestEditorNode::TransmissionTowerNodeApplicator::drawNode(ForestEditorNode* node)
{
	imnodes::BeginNode(node->_nodeType);

	imnodes::BeginNodeTitleBar();
	ImGui::TextUnformatted(ForestEditorNode::_forestNodeName[node->_nodeType]);
	imnodes::EndNodeTitleBar();

	ImGui::Image((void*)_nodeImage[node->_nodeType]->getID(), ImVec2(120, 154));

	ImGui::PushItemWidth(80.0f);
	{
		this->leaveSpace(1);

		imnodes::BeginStaticAttribute(0);

		ImGui::Checkbox("Enable Instancing", &node->_enableModel);

		imnodes::EndStaticAttribute();

		imnodes::BeginInputAttribute(getAttributeId(node->_nodeType, SCENE_ROOT, false));
		imnodes::EndInputAttribute();
	}

	imnodes::EndNode();
}