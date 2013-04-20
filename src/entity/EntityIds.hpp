#pragma once
#include <cstdint>

namespace EntityId {

enum EntityId : uint16_t {
#define ENTITY_DEF(id, name) name = id,
#include "EntityDefs.inc"
#undef ENTITY_DEF
	NUM_ENTITIES
};

}
