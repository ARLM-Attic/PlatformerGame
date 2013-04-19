#include "Component.hpp"
#include "ComponentManager.hpp"

void insertInChain(Component* chain, Component* comp) {
	Handle next = chain->next;
	chain->next = comp->self;
	comp->next = next;
}

void removeNextFromChain(ComponentManager& manager, Component* comp) {
	Handle removed_h = comp->next;
	Component* removed = manager.resolve(removed_h);

	comp->next = removed->next;
	removed->next = removed->self;
}

void removeFromChain(ComponentManager& manager, Component* target) {
	Component* c = target;
	for (; c->next != target->self; c = manager.resolve(c->next));
	removeNextFromChain(manager, c);
}
