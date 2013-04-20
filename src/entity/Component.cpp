#include "Component.hpp"
#include "ComponentManager.hpp"

void insertInChain(Component& chain, Component& comp) {
	assert(comp.next == comp.self);
	Handle next = chain.next;
	chain.next = comp.self;
	comp.next = next;
}

void removeNextFromChain(Component& comp) {
	Handle removed_h = comp.next;
	Component* removed = context.component_mgr->resolve(removed_h);

	comp.next = removed->next;
	removed->next = removed->self;
}

void removeFromChain(Component& chain, Component& target) {
	Component* c = &chain;
	for (; c->next != target.self; c = context.component_mgr->resolve(c->next));
	removeNextFromChain(*c);
}

Component* findInChain(const Component& chain, uint16_t component_id) {
	Handle first = chain.self;
	Handle i = chain.self;

	do {
		if (i.type == component_id) {
			return context.component_mgr->resolve(i);
		}
		i = context.component_mgr->resolve(i)->next;
	} while (i != first);

	return nullptr;
}
