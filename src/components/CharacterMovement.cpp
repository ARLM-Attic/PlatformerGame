#include "CharacterMovement.hpp"
#include "debug/DebugConstant.hpp"
#include "InputButtons.hpp"
#include "objects/BackgroundLayer.hpp"
#include "util/util.hpp"
#include "PositionComponent.hpp"
#include "BoundingRect.hpp"

static DebugConstant<float> MAX_MOVEMENT_VEL(2.25f, "Max move");
static DebugConstant<float> MOVEMENT_VEL_ACCEL(0.15f, "Move accel");
static DebugConstant<float> JUMP_VELOCITY(-6.f, "Jump vel");
static DebugConstant<float> FALL_GRAVITY(0.25f, "Fall grav");
static DebugConstant<float> JUMP_HIGH_GRAVITY(0.25f, "Jump high grav");
static DebugConstant<float> JUMP_LOW_GRAVITY(0.8f, "Jump low grav");

typedef std::array<int8_t, 16> TileHeightmap;
const std::array<TileHeightmap, 5> slope_data = {{
	/*  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
	{{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }},
	{{ 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15 }},
	{{  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 }},
	{{  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7 }},
	{{  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15 }}
}};

void CharacterMovement::init() {
	sub_position[0] = sub_position[1] = 0;
	on_ground = false;

	facing_direction = 1;
	move_velocity = 0.f;
	jump_velocity = 0.f;
	jump_grace_counter = 0;
}

void addSubposition(int position, int8_t sub_position, float displacement, int& out_position, int8_t& out_sub_position) {
	int pos_tmp = (position << 8) | (sub_position & 0xFF);
	pos_tmp += (int)(displacement * 256.f);
	out_position = pos_tmp >> 8;
	out_sub_position = pos_tmp & 0xFF;
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
		on_ground = false;
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

	auto searchCollision = [&](ivec2 layer_pos, ivec2 direction, int max_search, ivec2& out_result) -> bool {
		while (max_search > 0) {
			if (layer_pos.x < 0 || layer_pos.y < 0) {
				out_result = layer_pos;
				return false;
			}

			ivec2 map_coord = layer_pos / collision_layer.tile_size;
			uint16_t tile_id = collision_layer.map.get(map_coord.x, map_coord.y);
			ivec2 tile_pos = layer_pos % collision_layer.tile_size;
			int height = slope_data[tile_id >> 4][(tile_id & 1) ? 15 - tile_pos.x : tile_pos.x];

			int dist_from_edge = (direction.y < 0) ? tile_pos.y : collision_layer.tile_size.y - tile_pos.y - 1;
			if (dist_from_edge <= height) {
				out_result = layer_pos;
				return true;
			}

			--max_search;
			layer_pos = layer_pos + direction;

			/* INCOMPLETE CODE (potentially faster)
			if (height == 0) {
				if (vert_direction < 0) {
					max_search -= tile_pos.y + 1;
					layer_pos.y -= tile_pos.y + 1;
				} else {
					max_search -= collision_layer.tile_size.y - tile_pos.y;
					layer_pos.y += collision_layer.tile_size.y - tile_pos.y;
				}
			} else {
				if (vert_direction < 0) {
					int delta = tile_pos.y - max_search + 1 - height;
					if (delta < 0) {
						out_yresult = layer_pos.y - delta - max_search;
						return true;
					} else {
						return false;
					}
				} else {
					// TODO
				}
			}
			*/
		}
		return false;
	};

	static const int CLIMB_HEIGHT = 2;

	int delta_x;
	{
		static const Sensor left_sensors[]  = {SENSOR_HEAD_LEFT,  SENSOR_BODY_TOP_LEFT,  SENSOR_BODY_BOTTOM_LEFT};
		static const Sensor right_sensors[] = {SENSOR_HEAD_RIGHT, SENSOR_BODY_TOP_RIGHT, SENSOR_BODY_BOTTOM_RIGHT};

		int new_xpos; int8_t new_xsubpos;
		addSubposition(pos->position.x, sub_position[0], displacement.x, new_xpos, new_xsubpos);
		delta_x = new_xpos - pos->position.x;

		ivec2 layer_pos = pos->position - collision_layer.position;
		ivec2 coll_point;
		bool collided = false;
		
		int fixup = displacement.x < 0 ? -1 : 1;
		for (Sensor s : displacement.x < 0 ? left_sensors : right_sensors) {
			ivec2 sensor_pos = layer_pos + sensors[s] + mivec2(fixup, 0);
			if (searchCollision(sensor_pos, mivec2(fixup, 0), std::abs(delta_x) + 1, coll_point)) {
				collided = true;
				pos->position.x = coll_point.x - sensors[s].x + collision_layer.position.x - fixup;
				sub_position[0] = 0;
				move_velocity = 0;
				break;
			}
		}

		if (!collided) {
			pos->position.x = new_xpos;
			sub_position[0] = new_xsubpos;
		}
	}

	static const Sensor head_sensors[] = {SENSOR_HEAD_MIDDLE, SENSOR_HEAD_LEFT, SENSOR_HEAD_RIGHT};
	static const Sensor feet_sensors[] = {SENSOR_FEET_MIDDLE, SENSOR_FEET_LEFT, SENSOR_FEET_RIGHT};

	if (on_ground) {
		int max_climb_height = CLIMB_HEIGHT * std::abs(delta_x);

		ivec2 layer_pos = pos->position - collision_layer.position;
		ivec2 coll_point;
		int max_search = 2 * max_climb_height + 1;

		on_ground = false;
		
		for (Sensor s : feet_sensors) {
			ivec2 sensor_pos = layer_pos + sensors[s] + mivec2(0, -max_climb_height + 1);
			if (searchCollision(sensor_pos, ivec2_y, max_search, coll_point)) {
				pos->position.y = coll_point.y - sensors[s].y + collision_layer.position.y - 1;
				sub_position[1] = 0;
				on_ground = true;
				jump_velocity = 0.f;
				break;
			}
		}
	} else {
		int new_ypos; int8_t new_ysubpos;
		addSubposition(pos->position.y, sub_position[1], displacement.y, new_ypos, new_ysubpos);
		int delta_y = new_ypos - pos->position.y;

		ivec2 layer_pos = pos->position - collision_layer.position;
		ivec2 coll_point;
		int fixup = displacement.y > 0 ? 1 : -1;

		bool collided = false;

		for (Sensor s : ((displacement.y > 0) ? feet_sensors : head_sensors)) {
			ivec2 sensor_pos = layer_pos + sensors[s] + mivec2(0, fixup);
			if (searchCollision(sensor_pos, mivec2(0, fixup), std::abs(delta_y), coll_point)) {
				collided = true;
				pos->position.y = coll_point.y - sensors[s].y + collision_layer.position.y - fixup;
				sub_position[1] = 0;
				jump_velocity = 0;
				if (displacement.y > 0) {
					on_ground = true;
				}
				break;
			}
		}

		if (!collided) {
			pos->position.y = new_ypos;
			sub_position[1] = new_ysubpos;
		}
	}

	if (on_ground) {
		jump_grace_counter = 2;
	}
}
