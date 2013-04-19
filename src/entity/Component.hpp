#pragma once

#include "Handle.hpp"

struct ComponentManager;

struct Component {
	Handle self;
	Handle next;
};

void insertInChain(Component* chain, Component* comp);
void removeNextFromChain(ComponentManager& manager, Component* comp);
void removeFromChain(ComponentManager& manager, Component* target);
Component* findInChain(ComponentManager& manager, Component* chain, uint16_t component_id);

template <typename T>
T* findInChain(ComponentManager& manager, Component* chain) {
	return static_cast<T*>(findInChain(manager, chain, T::COMPONENT_ID));
}
