#pragma once

#include "render/SpriteBuffer.hpp"
#include "render/SpriteDb.hpp"
#include <vector>

struct RenderState {
	SpriteBufferIndices sprite_buffer_indices;
	SpriteBuffer tileset_buffer;
	SpriteBuffer ui_buffer;
};
