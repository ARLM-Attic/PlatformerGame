#include "CharacterMovement.hpp"
#include "debug/DebugConstant.hpp"
#include "game_types.hpp"
#include "objects/BackgroundLayer.hpp"
#include "util/util.hpp"
#include "PositionComponent.hpp"
#include "BoundingRect.hpp"

static DebugConstant<float> MAX_MOVEMENT_VEL(2.25f, "Max move");
static DebugConstant<float> MOVEMENT_VEL_ACCEL(0.15f, "Move accel");
static DebugConstant<float> JUMP_VELOCITY(-5.6f, "Jump vel");
static DebugConstant<float> FALL_GRAVITY(0.25f, "Fall grav");
static DebugConstant<float> JUMP_HIGH_GRAVITY(0.25f, "Jump high grav");
static DebugConstant<float> JUMP_LOW_GRAVITY(0.8f, "Jump low grav");
static DebugConstant<float> LEDGE_FALL_VELOCITY(1, "Ledge fall vel");

void CharacterMovement::init() {
	sub_position[0] = sub_position[1] = 0;
	on_ground = false;

	facing_direction = 1;
	move_velocity = 0.f;
	jump_velocity = 0.f;
	jump_grace_counter = 0;
}

void CharacterMovement::update(const InputButtons& input, const BackgroundLayer& collision_layer) {
	PositionComponent* pos = findInChain<PositionComponent>(*this);
	const BoundingRect* rect = findInChain<BoundingRect>(*this);

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

	auto checkCollision = [&](int d, int horz) {
		const int bottom = pos->position[1-d] + rect->size[1-d] - 1;
		bool collided = false;
		ivec2 sensor_pos;
		sensor_pos[d] = horz;
		for (int y = pos->position[1-d]; y < bottom; y += collision_layer.tile_size[1-d]) {
			sensor_pos[1-d] = y;
			collided = collided || collision_layer.getTileAt(sensor_pos);
		}
		sensor_pos[1-d] = bottom;
		collided = collided || collision_layer.getTileAt(sensor_pos);

		return collided;
	};

	for (int d = 0; d < 2; ++d) {
		if (displacement[d] != 0.f) {
			int pos_tmp = (pos->position[d] << 8) + sub_position[d];
			pos_tmp += (int)(displacement[d] * 256.f);
			pos->position[d] = pos_tmp >> 8;
			sub_position[d] = pos_tmp & 0xFF;

			//pos->position[d] += PositionFixed(displacement[d]);

			int horz = pos->position[d];
			if (displacement[d] > 0.f) {
				horz += rect->size[d] - 1;
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
					pos->position[d] = (((pos->position[d] - collision_layer.position[d]) / collision_layer.tile_size[d] + 1) * collision_layer.tile_size[d]) + collision_layer.position[d];
				} else if (displacement[d] > 0.f) {
					pos->position[d] = (((pos->position[d] - collision_layer.position[d]) / collision_layer.tile_size[d]) * collision_layer.tile_size[d]) + collision_layer.position[d];
				}
			}
		}
	}

	bool new_on_ground = checkCollision(1, pos->position[1] + rect->size[1]);
	if (on_ground && !new_on_ground && jump_velocity == 0) {
		jump_velocity = LEDGE_FALL_VELOCITY;
	}
	on_ground = new_on_ground;
	if (on_ground) {
		jump_grace_counter = 2;
	}
}
