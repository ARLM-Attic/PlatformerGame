#include "Component.hpp"
#include "ComponentManager.hpp"

void insertInChain(Component& chain, Component& comp) {
	assert(comp.next == comp.self);
	Handle next = chain.next;
	chain.next = comp.self;
	comp.next = next;
}

void removeNextFromChain(ComponentManager& manager, Component& comp) {
	Handle removed_h = comp.next;
	Component* removed = manager.resolve(removed_h);

	comp.next = removed->next;
	removed->next = removed->self;
}

void removeFromChain(ComponentManager& manager, Component& target) {
	Component* c = &target;
	for (; c->next != target.self; c = manager.resolve(c->next));
	removeNextFromChain(manager, *c);
}

const Component* findInChain(const ComponentManager& manager, const Component& chain, uint16_t component_id) {
	Handle first = chain.self;
	Handle i = chain.self;

	do {
		if (i.type == component_id) {
			return manager.resolve(i);
		}
		i = manager.resolve(i)->next;
	} while (i != first);

	return nullptr;
}

Component* findInChain(ComponentManager& manager, Component& chain, uint16_t component_id) {
	return const_cast<Component*>(findInChain(static_cast<const ComponentManager&>(manager), chain, component_id));
}
