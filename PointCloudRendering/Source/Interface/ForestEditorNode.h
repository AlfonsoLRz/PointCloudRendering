#pragma once

#include "Graphics/Application/TerrainParameters.h"
#include "Graphics/Core/Texture.h"
#include "imgui/imgui.h"
#include "imnodes/imnodes.h"

/**
*	@file ForestEditorNode.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 08/05/2019
*/

/**
*	@brief Wrapper for GUI graphics which allows the user to interact with the scene.
*/
class ForestEditorNode {
public:
	enum ForestNodeType
	{
		SCENE_ROOT, NOISE_SIMPLEX, NOISE_RAW, TERRAIN_SURFACE, FOREST, GRASS, WATER, WATCHER_MODEL, TRANSMISSION_TOWER_MODEL, NUM_NODE_TYPES
	};

	class ForestEditorNodeApplicator;
	
	typedef std::vector<std::unique_ptr<ForestEditorNodeApplicator>>	NodeApplicatorMap;
	typedef std::vector<std::string>									NodeImagePathMap;
	typedef std::vector<std::unique_ptr<Texture>>						TextureMap;
	typedef std::vector<std::pair<int, int>>							LinkMap;
	typedef TerrainParameters::TerrainUIParameters						SceneDescription;

public:
	const static char*				_forestNodeName[ForestNodeType::NUM_NODE_TYPES];

protected:
	const static uvec2				MAX_TERRAIN_SUBDIVISIONS;
	const static uvec2				MIN_TERRAIN_SUBDIVISIONS;
	const static vec2				MAX_TERRAIN_SIZE;
	const static vec2				MIN_TERRAIN_SIZE;
	const static uvec2				MAX_WATER_SUBDIVISIONS;
	const static uvec2				MIN_WATER_SUBDIVISIONS;
	const static GLuint				MAX_TREES;
	const static GLuint				MIN_TREES;
	const static GLuint				MAX_GRASS_SEEDS;
	const static GLuint				MIN_GRASS_SEEDS;
	const static float				MAX_NOISE_FREQUENCY;
	const static float				MIN_NOISE_FREQUENCY;

protected:
	static NodeApplicatorMap		_nodeApplicator;
	static NodeImagePathMap			_nodeImagePath;
	static TextureMap				_nodeImage;
	static SceneDescription			_sceneDescription;
	
	ForestNodeType					_nodeType;

public:
	// ---- Terrain
	uvec2		_numTerrainSubdivisions;
	vec2		_terrainSize;

	// ---- Water
	uvec2		_numWaterSubdivisions;

	// ---- Forest
	GLuint		_numTrees[3];

	// ---- Grass
	GLuint		_numGrassSeeds;

	// ---- Models
	bool		_enableModel;

	// ---- Noise
	float		_noiseFrequency;

protected:
	/**
	*	@return Map of node applicators. 
	*/
	static void buildApplicatorMap();

	/**
	*	@return Map of image paths for each type of node. 
	*/
	static void buildImagePathMap();

	/**
	*	@brief Loads images from system files to GPU. 
	*/
	static void loadTextures();

public:
	/**
	*	@brief Generic constructor. 
	*/
	ForestEditorNode(ForestNodeType type);

	/**
	*	@brief  
	*/
	virtual ~ForestEditorNode();

	/**
	*	@brief Draws node through ImGui commands. 
	*/
	void drawNode();

	/**
	*	@brief Gathers editor links.
	*/
	void updateLinks(LinkMap& links);

	/**
	*	@brief Initializes textures to be displayed on GUI. 
	*/
	static void initializeTextures();

	/**
	*	@return Binary description of struct.
	*/
	static bool saveDescriptionToBinary();

protected:
	/**
	*	@brief Drawing specification for any Forest node. 
	*/
	class ForestEditorNodeApplicator
	{
	protected:
		/**
		*	@return Identifier of attribute for current node and type of output.
		*/
		static int getAttributeId(const unsigned nodeType, const unsigned pinType, const bool output);
		
		/**
		*	@brief Calls ImGui::Spacing() for n times in a clean way.
		*/
		static void leaveSpace(const unsigned space);
	
	public:		
		/**
		*	@brief Draws ImGui node. 
		*/
		virtual void drawNode(ForestEditorNode* node) = 0;

		/**
		*	@brief Gathers links for output pins. 
		*/
		virtual void updateLinks(LinkMap& links) {}
	};

	/**
	*	@brief Drawing specification for a root node.
	*/
	class SceneRootNodeApplicator : public ForestEditorNodeApplicator
	{
	public:
		/**
		*	@brief Draws ImGui node.
		*/
		virtual void drawNode(ForestEditorNode* node);

		/**
		*	@brief Gathers links for output pins.
		*/
		virtual void updateLinks(LinkMap& links);
	};

	/**
	*	@brief Drawing specification for a Simplex noise node.
	*/
	class SimplexNodeApplicator : public ForestEditorNodeApplicator
	{
	public:
		/**
		*	@brief Draws ImGui node.
		*/
		virtual void drawNode(ForestEditorNode* node);
	};
	
	/**
	*	@brief Drawing specification for any Forest node.
	*/
	class SimpleNoiseApplicator : public ForestEditorNodeApplicator
	{
	public:
		/**
		*	@brief Draws ImGui node.
		*/
		virtual void drawNode(ForestEditorNode* node);
	};

	/**
	*	@brief Drawing specification for terrain surface node.
	*/
	class TerrainNodeApplicator : public ForestEditorNodeApplicator
	{
	public:
		/**
		*	@brief Draws ImGui node.
		*/
		virtual void drawNode(ForestEditorNode* node);

		/**
		*	@brief Gathers links for output pins.
		*/
		virtual void updateLinks(LinkMap& links);
	};

	/**
	*	@brief Drawing specification for water node.
	*/
	class WaterNodeApplicator : public ForestEditorNodeApplicator
	{
	public:
		/**
		*	@brief Draws ImGui node.
		*/
		virtual void drawNode(ForestEditorNode* node);
	};

	/**
	*	@brief Drawing specification for forest node.
	*/
	class ForestNodeApplicator : public ForestEditorNodeApplicator
	{
	public:
		/**
		*	@brief Draws ImGui node.
		*/
		virtual void drawNode(ForestEditorNode* node);
	};

	/**
	*	@brief Drawing specification for grass node.
	*/
	class GrassNodeApplicator : public ForestEditorNodeApplicator
	{
	public:
		/**
		*	@brief Draws ImGui node.
		*/
		virtual void drawNode(ForestEditorNode* node);
	};

	/**
	*	@brief Drawing specification for watcher model.
	*/
	class WatcherNodeApplicator : public ForestEditorNodeApplicator
	{
	public:
		/**
		*	@brief Draws ImGui node.
		*/
		virtual void drawNode(ForestEditorNode* node);
	};

	/**
	*	@brief Drawing specification for transmission tower model.
	*/
	class TransmissionTowerNodeApplicator : public ForestEditorNodeApplicator
	{
	public:
		/**
		*	@brief Draws ImGui node.
		*/
		virtual void drawNode(ForestEditorNode* node);
	};
};

