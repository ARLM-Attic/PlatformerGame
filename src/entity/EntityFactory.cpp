#include "EntityFactory.hpp"
#include "Component.hpp"
#include "ComponentManager.hpp"
#include "EntityIds.hpp"
#include "RenderState.hpp"
#include "objects/GameState.hpp"
#include "Context.hpp"

static Handle create_Player() {
	ComponentManager& manager = *context.component_mgr;

	auto& pos = *manager.createComponent<PositionComponent>();
	pos.position = mivec2(96, 192);

	auto& rect = *manager.createComponent<BoundingRect>();
	rect.origin = mivec2(0, 0);
	rect.size = mivec2(16, 32);

	auto& movement = *manager.createComponent<CharacterMovement>();
	movement.init();

	auto& spr = *manager.createComponent<SpriteComponent>();
	spr.layer = RenderState::LAYER_CHARACTER;
	spr.image = context.render_state->characters_sprdb.lookup("player_stand");
	spr.origin = mivec2(8, 0);

	insertInChain(pos, rect);
	insertInChain(pos, movement);
	insertInChain(pos, spr);

	return pos.self;
}

static std::array<Handle (*)(), EntityId::NUM_ENTITIES> entityFactories = {{
#define ENTITY_DEF(id, name) create_##name,
#include "EntityDefs.inc"
#undef ENTITY_DEF
}};

Handle createEntity(uint16_t entity_id) {
	assert(entity_id < EntityId::NUM_ENTITIES);
	return entityFactories[entity_id]();
}
