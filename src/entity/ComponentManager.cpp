#include "ComponentManager.hpp"
#include "Component.hpp"
#include <cassert>

ComponentManager::ComponentManager() {
#define COMPONENT_DEF(id, class_name) component_pools[ComponentId::class_name] = &component_pool_##class_name;
#include "components/ComponentDefs.inc"
#undef COMPONENT_DEF
}

const ObjectPool& ComponentManager::getPool(Handle h) const {
	assert(h.type <= ComponentId::NUM_COMPONENTS);
	return *(component_pools[h.type]);
}

const Component* ComponentManager::resolve(Handle h) const {
	return static_cast<const Component*>(getPool(h)[h]);
}

void ComponentManager::deleteChain(Component* chain) {
	const Handle start = chain->self;
	while (true) {
		Handle next = chain->next;
		getPool(chain->self).remove(chain->self);
		if (next == start) break;
		chain = resolve(next);
	}
}
