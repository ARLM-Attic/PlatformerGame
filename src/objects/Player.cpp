#include "Player.hpp"

#include "GameState.hpp"
#include "RenderState.hpp"
#include "render/SpriteDb.hpp"
#include "Camera.hpp"

void Player::init(const SpriteDb& sprite_db) {
	image = sprite_db.lookup("player_stand");
}

static bool checkCollisions(Position pos, float player_w, float player_h, const BackgroundLayer& layer) {
	return
		layer.getTileAt(pos) != 0 ||
		layer.getTileAt(pos + mvec2(player_w - 1, 0)) != 0 ||
		layer.getTileAt(pos + mvec2(0, player_h - 1)) != 0 ||
		layer.getTileAt(pos + mvec2(player_w - 1, player_h - 1)) != 0;
}

void Player::update(GameState& game_state, const InputButtons::Bitset& input) {
	static const int movement_speed = 4;

	vec2 vel = vec2_0;

	if (input.at(InputButtons::LEFT)) {
		vel.x -= movement_speed;
	}
	if (input.at(InputButtons::RIGHT)) {
		vel.x += movement_speed;
	}
	if (input.at(InputButtons::UP)) {
		vel.y -= movement_speed;
	}
	if (input.at(InputButtons::DOWN)) {
		vel.y += movement_speed;
	}

	static const int PLAYER_SIZE[2] = {32, 32};

	const BackgroundLayer& layer = game_state.player_layer;

	for (int d = 0; d < 2; ++d) {
		if (vel[d] != 0.f) {
			pos[d] += PositionFixed(vel[d]);

			PositionFixed horz = pos[d];
			if (vel[d] > 0.f) {
				horz += PLAYER_SIZE[d] - 1;
			}

			const PositionFixed bottom = pos[1-d] + PLAYER_SIZE[1-d] - 1;
			bool collided = false;
			Position sensor_pos;
			sensor_pos[d] = horz;
			for (PositionFixed y = pos[1-d]; y < bottom; y += layer.tile_size[1-d]) {
				sensor_pos[1-d] = y;
				collided = collided || layer.getTileAt(sensor_pos);
			}
			sensor_pos[1-d] = bottom;
			collided = collided || layer.getTileAt(sensor_pos);

			if (collided) {
				if (vel[d] < 0.f) {
					pos[d] = (((pos[d] - layer.position[d]) / fixed32_0(layer.tile_size[d])).trunc() + 1) * fixed32_0(layer.tile_size[d]) + layer.position[d];
				} else if (vel[d] > 0.f) {
					pos[d] = (((pos[d] + PLAYER_SIZE[d] - 1) - layer.position[d]) / fixed32_0(layer.tile_size[d])).trunc() * fixed32_0(layer.tile_size[d]) + layer.position[d] - PLAYER_SIZE[d];
				}
			}
		}
	}
}

void Player::draw(SpriteBuffer& buffer, const Camera& camera) const {
	Sprite spr;
	spr.img = image;
	spr.setPos(camera.transform(pos));
	buffer.append(spr);
}