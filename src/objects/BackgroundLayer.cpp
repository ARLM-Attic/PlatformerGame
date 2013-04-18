#include "BackgroundLayer.hpp"

#include "render/SpriteBuffer.hpp"
#include "Camera.hpp"

void BackgroundLayer::draw(SpriteBuffer& buffer, const Camera& camera) const {
	ivec2 map_offset = camera.transform(position);

	int tiles_in_x = intDivRoundUp(WINDOW_WIDTH, tile_size[0]) + 1;
	int tiles_in_y = intDivRoundUp(WINDOW_HEIGHT, tile_size[1]) + 1;

	int x_tiles_offset, y_tiles_offset;
	int x_pos_offset, y_pos_offset;
	intDiv(map_offset.x, tile_size[0], x_tiles_offset, x_pos_offset);
	intDiv(map_offset.y, tile_size[1], y_tiles_offset, y_pos_offset);

	drawTilemap(map, buffer, tile_size[0], tile_size[1], tiles_per_row, mivec2(x_pos_offset, y_pos_offset), -x_tiles_offset, -y_tiles_offset, tiles_in_x, tiles_in_y);
}

uint16_t BackgroundLayer::getTileAt(ivec2 world_pos) const {
	ivec2 v = getTilePosAt(world_pos);
	return map.get(v.x, v.y);
}

ivec2 BackgroundLayer::getTilePosAt(ivec2 world_pos) const {
	return mivec2(
		(world_pos.x - position.x) / tile_size[0],
		(world_pos.y - position.y) / tile_size[1]);
}

IntRect BackgroundLayer::getTileImgRect(uint16_t tile_id) const {
	IntRect r;
	r.x = tile_id % tiles_per_row * tile_size[0];
	r.y = tile_id / tiles_per_row * tile_size[1];
	r.w = tile_size[0];
	r.h = tile_size[1];
	return r;
}
