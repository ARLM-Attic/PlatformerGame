#pragma once

#include "Map.hpp"
#include "util/vec2.hpp"
#include "render/Sprite.hpp"

struct SpriteBuffer;
struct Camera;

struct BackgroundLayer {
	Map map;
	ivec2 tile_size;
	int tiles_per_row;

	ivec2 position;

	void draw(SpriteBuffer& buffer, const Camera& camera) const;

	uint16_t getTileAt(ivec2 world_pos) const;
	ivec2 getTilePosAt(ivec2 world_pos) const;
	IntRect getTileImgRect(uint16_t tile_id) const;
};
