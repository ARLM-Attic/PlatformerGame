#pragma once

struct GameState;
struct RenderState;
struct ComponentManager;

struct Context {
	GameState* game_state;
	RenderState* render_state;
	ComponentManager* component_mgr;
};

extern Context context;
