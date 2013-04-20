#pragma once
#include <cstdint>
#include "Handle.hpp"

struct ComponentManager;
struct Context;

Handle createEntity(ComponentManager& manager, const Context& context, uint16_t entity_id);