#pragma once

#include "Map.hpp"
#include "util/Position.hpp"

struct SpriteBuffer;
struct Camera;

struct BackgroundLayer {
	Map map;
	int tile_size[2];
	int tiles_per_row;

	Position position;

	void draw(SpriteBuffer& buffer, const Camera& camera) const;

	uint16_t getTileAt(Position world_pos) const;
	void getTilePosAt(Position world_pos, int& x, int& y) const;
};
