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
	enum EditorMode {
		MODE_TILES,
		MODE_TILECHOOSER
	};

	ivec2 mouse_coords; // world mouse coords
	EditorMode mode;

	// MODE_TILES
	uint16_t current_tile_id;

	// MODE_TILECHOOSER
	ivec2 saved_mouse_pos;

	void init() {
		mode = MODE_TILES;
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

	if (editor_state.mode == EditorState::MODE_TILES) {
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

		if (input.pressed[InputButtons::EDITOR_CHOOSE_TILE]) {
			editor_state.saved_mouse_pos = game_state.input.mouse_pos;

			const BackgroundLayer& layer = game_state.player_layer;

			ivec2 new_mouse;
			intDiv(editor_state.current_tile_id, layer.tiles_per_row, new_mouse.y, new_mouse.x);
			new_mouse = new_mouse * layer.tile_size + (layer.tile_size / 2);
			game_state.input.warpMouse(new_mouse);

			editor_state.mode = EditorState::MODE_TILECHOOSER;
		} else if (input.pressed[InputButtons::EDITOR_PICK_TILE]) {
			editor_state.current_tile_id = game_state.player_layer.getTileAt(editor_state.mouse_coords);
		} else if (input.held[InputButtons::EDITOR_PLACE_TILE]) {
			ivec2 map_coord = game_state.player_layer.getTilePosAt(editor_state.mouse_coords);
			game_state.player_layer.map.set(map_coord.x, map_coord.y, editor_state.current_tile_id);
		}
	} else if (editor_state.mode == EditorState::MODE_TILECHOOSER) {
		const BackgroundLayer& layer = game_state.player_layer;

		ivec2 selected_tile = game_state.input.mouse_pos / layer.tile_size;
		selected_tile.x = clamp(0, selected_tile.x, layer.tiles_per_row - 1);

		uint16_t new_tile_id = selected_tile.y * layer.tiles_per_row + selected_tile.x;
		if (new_tile_id >= layer.num_tiles) {
			new_tile_id = layer.num_tiles - 1;
		}
		editor_state.current_tile_id = new_tile_id;

		if (input.released[InputButtons::EDITOR_CHOOSE_TILE]) {
			game_state.input.warpMouse(editor_state.saved_mouse_pos);
			editor_state.mode = EditorState::MODE_TILES;
		}
	}
}

void editorDraw(const GameState& game_state, RenderState& render_state) {
	std::string status_text;

	if (editor_state.mode == EditorState::MODE_TILES) {
		// Draw world
		render_state.drawSprites();
		render_state.drawTileLayers();

		// Draw tile cursor
		Sprite tile_spr;
		tile_spr.img = game_state.player_layer.getTileImgRect(editor_state.current_tile_id);
		tile_spr.pos = intRoundTo(editor_state.mouse_coords - game_state.player_layer.position, game_state.player_layer.tile_size) + game_state.player_layer.position;
		tile_spr.pos = game_state.camera.transform(tile_spr.pos);
		tile_spr.color = makeColor(128, 128, 128, 64);
		render_state.sprite_buffers[RenderState::LAYER_TILESET].append(tile_spr);

		// Draw status line text
		boost::format status_text_fmt("%1% - X: %2% Y: %3%");
		status_text = str(status_text_fmt % "TILES" % editor_state.mouse_coords.x % editor_state.mouse_coords.y);
	} else if (editor_state.mode == EditorState::MODE_TILECHOOSER) {
		const TextureInfo& texture = render_state.sprite_buffers[RenderState::LAYER_TILESET].texture;

		Sprite spr;
		spr.img.x = spr.img.y = 0;
		spr.img.w = texture.width;
		spr.img.h = texture.height;
		spr.pos = ivec2_0;
		render_state.sprite_buffers[RenderState::LAYER_TILESET].append(spr);

		boost::format status_text_fmt("TILE ID: %1%");
		status_text = str(status_text_fmt % editor_state.current_tile_id);
	}

	drawString(0, WINDOW_HEIGHT - ui_font.char_h, status_text, render_state.sprite_buffers[RenderState::LAYER_UI], ui_font, TextAlignment::left, color_white);
}
