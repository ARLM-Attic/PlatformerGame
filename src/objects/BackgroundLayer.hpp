#pragma once

#include "Map.hpp"
#include "util/vec2.hpp"

struct SpriteBuffer;
struct Camera;

struct BackgroundLayer {
	Map map;
	int tile_size[2];
	int tiles_per_row;

	ivec2 position;

	void draw(SpriteBuffer& buffer, const Camera& camera) const;

	uint16_t getTileAt(ivec2 world_pos) const;
	ivec2 getTilePosAt(ivec2 world_pos) const;
};
