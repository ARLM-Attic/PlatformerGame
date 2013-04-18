#include "Player.hpp"

#include "GameState.hpp"
#include "RenderState.hpp"
#include "render/SpriteDb.hpp"
#include "Camera.hpp"
#include "game_types.hpp"
#include <iostream>
#include "debug/DebugConstant.hpp"

static DebugConstant<float> MAX_MOVEMENT_VEL(2.25f, "Max move");
static DebugConstant<float> MOVEMENT_VEL_ACCEL(0.15f, "Move accel");
static DebugConstant<float> JUMP_VELOCITY(-5.6f, "Jump vel");
static DebugConstant<float> FALL_GRAVITY(0.25f, "Fall grav");
static DebugConstant<float> JUMP_HIGH_GRAVITY(0.25f, "Jump high grav");
static DebugConstant<float> JUMP_LOW_GRAVITY(0.8f, "Jump low grav");
static DebugConstant<float> LEDGE_FALL_VELOCITY(1, "Ledge fall vel");

void Player::init(const SpriteDb& sprite_db) {
	image = sprite_db.lookup("player_stand");
	on_ground = false;
	size = mivec2(16, 32);

	facing_direction = 1;
	move_velocity = 0.f;
	jump_velocity = 0.f;
	jump_grace_counter = 0;
}

void Player::update(GameState& game_state, const InputButtons& input) {
	vec2 displacement = vec2_0;

	if ((!input.held[InputButtons::LEFT] && move_velocity < 0) || (!input.held[InputButtons::RIGHT] && move_velocity > 0)) {
		move_velocity = stepTowards(move_velocity, 0.f, on_ground ? 0.2f : 0.1f);
	}

	if (input.held[InputButtons::LEFT] == input.held[InputButtons::RIGHT]) {
	} else if (input.held[InputButtons::LEFT]) {
		move_velocity -= MOVEMENT_VEL_ACCEL;
		facing_direction = -1;
	} else if (input.held[InputButtons::RIGHT]) {
		move_velocity += MOVEMENT_VEL_ACCEL;
		facing_direction = 1;
	}
	move_velocity = clamp(-MAX_MOVEMENT_VEL.value, move_velocity, MAX_MOVEMENT_VEL.value);

	if (input.pressed[InputButtons::UP] && jump_grace_counter > 0) {
		jump_velocity = JUMP_VELOCITY;
		jump_grace_counter = 0;
	}
	if (jump_grace_counter > 0) {
		jump_grace_counter--;
	}

	if (!on_ground) {
		if (jump_velocity > 0.f) {
			jump_velocity += FALL_GRAVITY;
		} else if (input.held[InputButtons::UP]) {
			jump_velocity += JUMP_HIGH_GRAVITY;
		} else {
			jump_velocity += JUMP_LOW_GRAVITY;
		}
	}
	jump_velocity = clamp(-12.f, jump_velocity, 12.f);

	displacement.x += move_velocity;
	displacement.y += jump_velocity;

	const BackgroundLayer& layer = game_state.player_layer;

	auto checkCollision = [&](int d, int horz) {
		const int bottom = pos[1-d].integer() + size[1-d] - 1;
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
				horz += size[d] - 1;
			}

			if (checkCollision(d, horz)) {
				if (d == 0) {
					move_velocity = 0.f;
				} else if (d == 1) {
					if (jump_velocity > 0) {
						jump_velocity = 0;
					} else if (jump_velocity < -1) {
						jump_velocity = -1;
					}
				}

				if (displacement[d] < 0.f) {
					pos[d] = (((pos[d].integer() - layer.position[d]) / layer.tile_size[d] + 1) * layer.tile_size[d]) + layer.position[d];
				} else if (displacement[d] > 0.f) {
					pos[d] = (((pos[d].integer() - layer.position[d]) / layer.tile_size[d]) * layer.tile_size[d]) + layer.position[d];
				}
			}
		}
	}

	bool new_on_ground = checkCollision(1, pos[1].integer() + size[1]);
	if (on_ground && !new_on_ground && jump_velocity == 0) {
		jump_velocity = LEDGE_FALL_VELOCITY;
	}
	on_ground = new_on_ground;
	if (on_ground) {
		jump_grace_counter = 2;
	}
}

void Player::draw(SpriteBuffer& buffer, const Camera& camera) const {
	Sprite spr;
	spr.img = image;
	spr.setPos(camera.transform(pos.integer() + mivec2(-8, 0)));
	buffer.append(spr);
}
