#pragma once
#include "entity/Component.hpp"
#include "ComponentIds.hpp"

struct InputButtons;
struct BackgroundLayer;

struct CharacterMovement : Component {
	static const uint16_t COMPONENT_ID = ComponentId::CharacterMovement;

	int8_t sub_position[2];
	bool on_ground;

	int facing_direction;
	float move_velocity;
	float jump_velocity;
	int jump_grace_counter;

	void init();
	void update(const InputButtons& input, const BackgroundLayer& collision_layer);
};
