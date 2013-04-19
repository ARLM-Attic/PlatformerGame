#pragma once

#include "Handle.hpp"
#include "components/ComponentIds.hpp"
#include "ComponentPool.hpp"
#include <array>

struct Component;

#include "components/PositionComponent.hpp"
#include "components/SpriteComponent.hpp"

struct ComponentManager {
#define COMPONENT_DEF(id, class_name) ComponentPool<class_name> component_pool_##class_name;
#include "components/ComponentDefs.inc"
#undef COMPONENT_DEF
	std::array<ObjectPool*, ComponentId::NUM_COMPONENTS> component_pools;

	ComponentManager();

	ObjectPool& getPool(Handle h);
	Component* resolve(Handle h);
	void deleteChain(Component* chain);
};
