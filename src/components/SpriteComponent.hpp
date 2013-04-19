#pragma once
#include "entity/Component.hpp"
#include "ComponentIds.hpp"
#include "render/Sprite.hpp"
#include "util/vec2.hpp"

struct SpriteComponent : Component {
	static const int COMPONENT_ID = ComponentId::SpriteComponent;

	IntRect image;
	ivec2 origin;
	int layer;
};