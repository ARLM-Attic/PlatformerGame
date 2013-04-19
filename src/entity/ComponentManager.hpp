#pragma once

#include "Handle.hpp"
#include "components/ComponentIds.hpp"
#include "ComponentPool.hpp"
#include <array>

struct Component;

#include "components/PositionComponent.hpp"
#include "components/SpriteComponent.hpp"

struct ComponentManager {
#define COMPONENT_DEF(id, class_name) \
	ComponentPool<class_name> component_pool_##class_name; \
	ComponentPool<class_name>& getPoolForType(class_name*) { return component_pool_##class_name; }
#include "components/ComponentDefs.inc"
#undef COMPONENT_DEF
	std::array<ObjectPool*, ComponentId::NUM_COMPONENTS> component_pools;

	ComponentManager();

	const ObjectPool& getPool(Handle h) const;
	ObjectPool& getPool(Handle h) { return const_cast<ObjectPool&>(static_cast<const ComponentManager*>(this)->getPool(h)); }

	const Component* resolve(Handle h) const;
	Component* resolve(Handle h) { return const_cast<Component*>(static_cast<const ComponentManager*>(this)->resolve(h)); }

	template <typename T>
	T* createComponent() {
		ComponentPool<T>& pool = getPoolForType(static_cast<T*>(nullptr));
		Handle h = pool.insert();
		return pool[h];
	}

	void deleteChain(Component* chain);
};
