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
};