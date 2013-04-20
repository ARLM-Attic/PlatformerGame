#pragma once

#include "Handle.hpp"
#include "Context.hpp"

struct ComponentManager;

struct Component {
	Handle self;
	Handle next;
};

void insertInChain(Component& chain, Component& comp);
void removeNextFromChain(Component& comp);
void removeFromChain(Component& chain, Component& target);
Component* findInChain(const Component& chain, uint16_t component_id);

template <typename T>
T* findInChain(Component& chain) {
	return static_cast<T*>(findInChain(chain, T::COMPONENT_ID));
}

template <typename T>
const T* findInChain(const Component& chain) {
	return static_cast<const T*>(findInChain(chain, T::COMPONENT_ID));
}

template <typename T>
T* findInChain(Handle chain) {
	Component* c = context.component_mgr->resolve(chain);
	if (!c) {
		return nullptr;
	} else {
		return static_cast<T*>(findInChain(*c, T::COMPONENT_ID));
	}
}
