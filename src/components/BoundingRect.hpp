#pragma once
#include "entity/Component.hpp"
#include "ComponentIds.hpp"
#include "util/vec2.hpp"

struct BoundingRect : Component {
	static const uint16_t COMPONENT_ID = ComponentId::BoundingRect;

	ivec2 origin;
	ivec2 size;
};
