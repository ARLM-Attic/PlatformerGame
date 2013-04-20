#include "Editor.hpp"
#include "DebugConstant.hpp"
#include "util/vec2.hpp"
#include "objects/GameState.hpp"
#include "RenderState.hpp"
#include "render/text.hpp"
#include "hud.hpp"
#include <boost/format.hpp>

static DebugConstant<bool> EDITOR_ENABLED(false, "Editor enabled");

struct EditorState {
	ivec2 mouse_coords;
	uint16_t current_tile_id;

	void init() {
		current_tile_id = 1;
	}
};

EditorState editor_state;

void editorInit() {
	editor_state.init();
}

bool editorIsEnabled() {
	return EDITOR_ENABLED;
}

void editorUpdate(GameState& game_state) {
	const InputButtons& input = game_state.input;

	editor_state.mouse_coords = game_state.camera.inverse_transform(game_state.input.mouse_pos);

	int scroll_speed = 4;
	if (input.held[InputButtons::EDITOR_SCROLL_LEFT]) {
		game_state.camera.pos.x -= scroll_speed;
	}
	if (input.held[InputButtons::EDITOR_SCROLL_RIGHT]) {
		game_state.camera.pos.x += scroll_speed;
	}
	if (input.held[InputButtons::EDITOR_SCROLL_UP]) {
		game_state.camera.pos.y -= scroll_speed;
	}
	if (input.held[InputButtons::EDITOR_SCROLL_DOWN]) {
		game_state.camera.pos.y += scroll_speed;
	}

	if (input.pressed[InputButtons::EDITOR_PICK_TILE]) {
		editor_state.current_tile_id = game_state.player_layer.getTileAt(editor_state.mouse_coords);
	}
	if (input.held[InputButtons::EDITOR_PLACE_TILE]) {
		ivec2 map_coord = game_state.player_layer.getTilePosAt(editor_state.mouse_coords);
		game_state.player_layer.map.set(map_coord.x, map_coord.y, editor_state.current_tile_id);
	}
}

void editorDraw(const GameState& game_state, RenderState& render_state) {
	Sprite tile_spr;
	tile_spr.img = game_state.player_layer.getTileImgRect(editor_state.current_tile_id);
	tile_spr.pos = intRoundTo(editor_state.mouse_coords - game_state.player_layer.position, game_state.player_layer.tile_size) + game_state.player_layer.position;
	tile_spr.pos = game_state.camera.transform(tile_spr.pos);
	tile_spr.color = makeColor(128, 128, 128, 64);
	render_state.sprite_buffers[RenderState::LAYER_TILESET].append(tile_spr);

	boost::format status_text_fmt("%1% - X: %2% Y: %3%");
	std::string status_text = (status_text_fmt % "TILES" % editor_state.mouse_coords.x % editor_state.mouse_coords.y).str();
	drawString(0, WINDOW_HEIGHT - ui_font.char_h, status_text, render_state.sprite_buffers[RenderState::LAYER_UI], ui_font, TextAlignment::left, color_white);
}
