#pragma once

#include "Handle.hpp"
#include "ObjectPool.hpp"
#include "ComponentIds.hpp"
#include <array>

struct Component;

struct ComponentManager {
	std::array<ObjectPool, ComponentId::NUM_COMPONENTS> component_pools;

	ObjectPool& getPool(Handle h);
	Component* resolve(Handle h);
	void deleteChain(Component* chain);
};
