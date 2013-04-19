#pragma once
#include <cstdint>

namespace ComponentId {

enum ComponentId : uint16_t {
#define COMPONENT_DEF(id, name) name = id,
#include "ComponentDefs.inc"
#undef COMPONENT_DEF
	NUM_COMPONENTS
};

}
