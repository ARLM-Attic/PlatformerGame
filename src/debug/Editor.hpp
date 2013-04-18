#pragma once

struct GameState;
struct RenderState;

void editorInit();
bool editorIsEnabled();

void editorUpdate(GameState& game_state);
void editorDraw(const GameState& game_state, RenderState& render_state);
