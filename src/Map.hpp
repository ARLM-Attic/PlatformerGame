#pragma once

#include <vector>
#include <cstdint>
#include <cassert>
#include <string>

struct SpriteBuffer;

struct Map {
	int width, height;
	std::vector<uint16_t> data;

	Map()
		: width(0), height(0)
	{}

	uint16_t get(int x, int y) const {
		assert(x < width);
		assert(y < height);
		return data[y * width + x];
	}
};

Map loadMap(const std::string& filename);
void drawTilemap(const Map& map, SpriteBuffer& buffer, int tile_w, int tile_h, int tiles_per_row, int screen_x, int screen_y, int map_xoffset, int map_yoffset, int map_wtiles, int map_htiles);
