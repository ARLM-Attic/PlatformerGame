#include "Map.hpp"
#include "render/SpriteBuffer.hpp"
#include "util/util.hpp"
#include "util/resources.hpp"

#include <fstream>
#include <sstream>

Map loadMap(const std::string& filename) {
	std::ifstream f(data_path + filename);
	assert(f);
	std::string linestr;

	Map map;

	std::getline(f, linestr);
	std::istringstream liness(linestr);
	liness >> map.width >> map.height;

	map.data.reserve(map.width * map.height);
	for (int y = 0; y < map.height; ++y) {
		std::getline(f, linestr);
		liness.str(linestr);
		liness.clear();
		for (int x = 0; x < map.width; ++x) {
			uint16_t tile;
			liness >> tile;
			map.data.push_back(tile);
		}
	}

	return map;
}

void drawTilemap(const Map& map, SpriteBuffer& buffer, int tile_w, int tile_h, int tiles_per_row, int screen_x, int screen_y, int map_xoffset, int map_yoffset, int map_wtiles, int map_htiles) {
	Sprite spr;
	spr.img.w = tile_w;
	spr.img.h = tile_h;

	if (map_wtiles < 0)
		map_wtiles = map.width;
	if (map_htiles < 0)
		map_htiles = map.height;

	int xbegin = clamp(0, map_xoffset, map.width);
	int ybegin = clamp(0, map_yoffset, map.height);
	int xend = clamp(0, map_xoffset + map_wtiles, map.width);
	int yend = clamp(0, map_yoffset + map_htiles, map.height);

	for (int tiley = ybegin; tiley < yend; ++tiley) {
		for (int tilex = xbegin; tilex < xend; ++tilex) {
			uint16_t tile_id = map.get(tilex, tiley);
			if (tile_id == 0)
				continue;

			spr.x = (tilex - map_xoffset) * tile_w + screen_x;
			spr.y = (tiley - map_yoffset) * tile_h + screen_y;
			spr.img.x = tile_id % tiles_per_row * tile_w;
			spr.img.y = tile_id / tiles_per_row * tile_h;

			buffer.append(spr);
		}
	}
}
