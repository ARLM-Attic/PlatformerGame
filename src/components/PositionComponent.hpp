#pragma once
#include "entity/Component.hpp"
#include "ComponentIds.hpp"
#include "util/vec2.hpp"

struct PositionComponent : Component {
	static const int COMPONENT_ID = ComponentId::PositionComponent;

	ivec2 position;
};