#pragma once

#include "Handle.hpp"

struct ComponentManager;

struct Component {
	Handle self;
	Handle next;
};

void insertInChain(Component& chain, Component& comp);
void removeNextFromChain(ComponentManager& manager, Component& comp);
void removeFromChain(ComponentManager& manager, Component& target);
Component* findInChain(ComponentManager& manager, Component& chain, uint16_t component_id);
const Component* findInChain(const ComponentManager& manager, const Component& chain, uint16_t component_id);

template <typename T>
T* findInChain(ComponentManager& manager, Component& chain) {
	return static_cast<T*>(findInChain(manager, chain, T::COMPONENT_ID));
}

template <typename T>
const T* findInChain(const ComponentManager& manager, const Component& chain) {
	return static_cast<const T*>(findInChain(manager, chain, T::COMPONENT_ID));
}

template <typename T>
T* findInChain(ComponentManager& manager, Handle chain) {
	Component* c = manager.resolve(chain);
	if (!c) {
		return nullptr;
	} else {
		return static_cast<T*>(findInChain(manager, *c, T::COMPONENT_ID));
	}
}

template <typename T>
const T* findInChain(const ComponentManager& manager, Handle chain) {
	const Component* c = manager.resolve(chain);
	if (!c) {
		return nullptr;
	} else {
		return static_cast<const T*>(findInChain(manager, *c, T::COMPONENT_ID));
	}
}
