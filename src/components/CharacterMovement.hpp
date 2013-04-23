#pragma once
#include "entity/Component.hpp"
#include "ComponentIds.hpp"
#include "util/vec2.hpp"
#include <array>
#include <cstdint>

struct InputButtons;
struct BackgroundLayer;

struct CharacterMovement : Component {
	static const uint16_t COMPONENT_ID = ComponentId::CharacterMovement;

	enum Sensor {
		SENSOR_HEAD_LEFT,
		SENSOR_HEAD_MIDDLE,
		SENSOR_HEAD_RIGHT,
		SENSOR_BODY_LEFT,
		SENSOR_BODY_RIGHT,
		SENSOR_FEET_LEFT,
		SENSOR_FEET_MIDDLE,
		SENSOR_FEET_RIGHT,
		SENSOR_MAX
	};

	std::array<ivec2, SENSOR_MAX> sensors;

	int8_t sub_position[2];
	bool on_ground;

	int facing_direction;
	float move_velocity;
	float jump_velocity;
	int jump_grace_counter;

	void init();
	void update(const InputButtons& input, const BackgroundLayer& collision_layer);
};
