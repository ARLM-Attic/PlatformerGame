#include "Player.hpp"

#include "GameState.hpp"
#include "RenderState.hpp"
#include "render/SpriteDb.hpp"
#include "Camera.hpp"
#include <iostream>

void Player::init(const SpriteDb& sprite_db) {
	image = sprite_db.lookup("player_stand");
	on_ground = false;

	move_velocity = 0.f;
	jump_velocity = 0.f;
}

void Player::update(GameState& game_state, const InputButtons::Bitset& input) {
	static const float max_movement_vel = 4;
	static const float movement_vel_accel = 0.25f;

	vec2 displacement = vec2_0;

	if (input.at(InputButtons::LEFT)) {
		move_velocity -= movement_vel_accel;
	}
	if (input.at(InputButtons::RIGHT)) {
		move_velocity += movement_vel_accel;
	}
	if (!input.at(InputButtons::LEFT) && !input.at(InputButtons::RIGHT)) {
		move_velocity = stepTowards(move_velocity, 0.f, 0.4f);
	}
	move_velocity = clamp(-max_movement_vel, move_velocity, max_movement_vel);

	if (input.at(InputButtons::UP) && on_ground) {
		std::cerr << "jump!!!" << std::endl;
		jump_velocity = -6;
	}
	/*
	if (input.at(InputButtons::DOWN)) {
		vel.y += movement_speed;
	}
	*/

	if (!on_ground) {
		jump_velocity += 0.2f;
	}
	jump_velocity = clamp(-16.f, jump_velocity, 16.f);

	displacement.x += move_velocity;
	displacement.y += jump_velocity;

	static const int PLAYER_SIZE[2] = {32, 32};

	const BackgroundLayer& layer = game_state.player_layer;

	auto checkCollision = [&](int d, int horz) {
		const int bottom = pos[1-d].integer() + PLAYER_SIZE[1-d] - 1;
		bool collided = false;
		ivec2 sensor_pos;
		sensor_pos[d] = horz;
		for (int y = pos[1-d].integer(); y < bottom; y += layer.tile_size[1-d]) {
			sensor_pos[1-d] = y;
			collided = collided || layer.getTileAt(sensor_pos);
		}
		sensor_pos[1-d] = bottom;
		collided = collided || layer.getTileAt(sensor_pos);

		return collided;
	};

	for (int d = 0; d < 2; ++d) {
		if (displacement[d] != 0.f) {
			pos[d] += PositionFixed(displacement[d]);

			int horz = pos[d].integer();
			if (displacement[d] > 0.f) {
				horz += PLAYER_SIZE[d] - 1;
			}

			if (checkCollision(d, horz)) {
				if (d == 0) {
					move_velocity = 0.f;
				} else if (d == 1) {
					jump_velocity = 0.f;
				}

				if (displacement[d] < 0.f) {
					pos[d] = (((pos[d].integer() - layer.position[d]) / layer.tile_size[d] + 1) * layer.tile_size[d]) + layer.position[d];
				} else if (displacement[d] > 0.f) {
					pos[d] = (((pos[d].integer() - layer.position[d]) / layer.tile_size[d]) * layer.tile_size[d]) + layer.position[d];
				}
				assert(pos[d] == pos[d].trunc());
			}
		}
	}

	on_ground = checkCollision(1, pos[1].integer() + PLAYER_SIZE[1]);
}

void Player::draw(SpriteBuffer& buffer, const Camera& camera) const {
	//assert(pos[0] == pos[0].trunc());
	Sprite spr;
	spr.img = image;
	spr.setPos(camera.transform(pos.integer()));
	buffer.append(spr);
}
