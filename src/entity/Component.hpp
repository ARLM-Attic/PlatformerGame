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