#pragma once

struct GameState;
struct RenderState;

struct Context {
	GameState* game_state;
	RenderState* render_state;
};
