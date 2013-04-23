#pragma once

#include "render/SpriteBuffer.hpp"
#include "render/SpriteDb.hpp"
#include <vector>

struct RenderState {
	
	enum SpriteLayer {
		LAYER_CHARACTER,
		LAYER_TILES_FG,
		LAYER_TILES_ACTION,
		LAYER_UI,
		LAYER_MAX
	};

	SpriteBufferIndices sprite_buffer_indices;
	std::array<SpriteBuffer, LAYER_MAX> sprite_buffers;
	SpriteDb characters_sprdb;

	void clearBuffers();
	void drawSprites();
	void drawTileLayers();
	void drawFrametime();
	void submitSprites();
};
