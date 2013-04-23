#include "Editor.hpp"
#include "DebugConstant.hpp"
#include "util/vec2.hpp"
#include "objects/GameState.hpp"
#include "RenderState.hpp"
#include "render/text.hpp"
#include "hud.hpp"
#include "components/CharacterMovement.hpp"
#include <boost/format.hpp>
#include <memory>

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
	GameState::LevelLayer current_layer;

	// MODE_TILECHOOSER
	ivec2 saved_mouse_pos;

	void init() {
		mode = MODE_TILES;
		current_tile_id = 1;
		current_layer = GameState::LAYER_FOREGROUND;
	}
};

EditorState editor_state;

static RenderState::SpriteLayer getSpriteLayerForLevelLayer(GameState::LevelLayer layer) {
	switch (layer) {
	case GameState::LAYER_ACTION: return RenderState::LAYER_TILES_ACTION;
	case GameState::LAYER_FOREGROUND: return RenderState::LAYER_TILES_FG;
	default: assert(false); return RenderState::LAYER_MAX;
	}
}

static void createHeightmapTileset() {
	static const int TILE_SIZE = 16;

	int img_width = 16 * TILE_SIZE;
	int img_height = slope_data.size() * TILE_SIZE;
	size_t img_stride = img_width * 4;
	std::vector<uint8_t> tex_data(img_stride * img_height, 0);

	for (size_t slope_id = 0; slope_id < slope_data.size(); ++slope_id) {
		for (int flags = 0; flags < 16; ++flags) {
			uint16_t i = slope_id * 16 + flags;
			bool flipped = (flags & 1) != 0;
			bool passable = (flags & 2) != 0;
			const TileHeightmap& heightmap = slope_data[slope_id];
			for (int x = 0; x < TILE_SIZE; ++x) {
				size_t col_start = (slope_id * TILE_SIZE) * img_stride + (flags * TILE_SIZE + x) * 4;

				int y = TILE_SIZE - 1 - heightmap[flipped ? TILE_SIZE - 1 - x : x];
				for (; y < TILE_SIZE; ++y) {
					size_t offs = col_start + y * img_stride;
					tex_data[offs + 0] = 127;
					tex_data[offs + 1] = passable ? 127 : 0;
					tex_data[offs + 2] = 0;
					tex_data[offs + 3] = 127;
				}
			}
		}
	}

	context.render_state->sprite_buffers[RenderState::LAYER_TILES_ACTION].texture = std::make_shared<TextureInfo>(loadTexture(img_width, img_height, tex_data.data()));
}

void editorInit() {
	editor_state.init();
	createHeightmapTileset();
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

		if (input.pressed[InputButtons::EDITOR_NEXT_LAYER]) {
			editor_state.current_layer = GameState::LevelLayer((editor_state.current_layer + 1) % GameState::LAYER_MAX);
		}

		BackgroundLayer& layer = game_state.level_layers[editor_state.current_layer];

		if (input.pressed[InputButtons::EDITOR_CHOOSE_TILE]) {
			editor_state.saved_mouse_pos = game_state.input.mouse_pos;

			ivec2 new_mouse;
			intDiv(editor_state.current_tile_id, layer.tiles_per_row, new_mouse.y, new_mouse.x);
			new_mouse = new_mouse * layer.tile_size + (layer.tile_size / 2);
			game_state.input.warpMouse(new_mouse);

			editor_state.mode = EditorState::MODE_TILECHOOSER;
		} else if (input.pressed[InputButtons::EDITOR_PICK_TILE]) {
			editor_state.current_tile_id = layer.getTileAt(editor_state.mouse_coords);
		} else if (input.held[InputButtons::EDITOR_PLACE_TILE]) {
			ivec2 map_coord = layer.getTilePosAt(editor_state.mouse_coords);
			layer.map.set(map_coord.x, map_coord.y, editor_state.current_tile_id);
		}
	} else if (editor_state.mode == EditorState::MODE_TILECHOOSER) {
		const BackgroundLayer& layer = game_state.level_layers[editor_state.current_layer];

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
		if (editor_state.current_layer == GameState::LAYER_ACTION) {
			game_state.level_layers[GameState::LAYER_ACTION].draw(render_state.sprite_buffers[RenderState::LAYER_TILES_ACTION], game_state.camera);
		}

		const BackgroundLayer& layer = game_state.level_layers[editor_state.current_layer];

		// Draw tile cursor
		Sprite tile_spr;
		tile_spr.img = layer.getTileImgRect(editor_state.current_tile_id);
		tile_spr.pos = intRoundTo(editor_state.mouse_coords - layer.position, layer.tile_size) + layer.position;
		tile_spr.pos = game_state.camera.transform(tile_spr.pos);
		tile_spr.color = makeColor(128, 128, 128, 64);
		render_state.sprite_buffers[getSpriteLayerForLevelLayer(editor_state.current_layer)].append(tile_spr);

		// Draw status line text
		boost::format status_text_fmt("%1% - X: %2% Y: %3%");
		status_text = str(status_text_fmt % "TILES" % editor_state.mouse_coords.x % editor_state.mouse_coords.y);
	} else if (editor_state.mode == EditorState::MODE_TILECHOOSER) {
		const TextureInfo& texture = *render_state.sprite_buffers[getSpriteLayerForLevelLayer(editor_state.current_layer)].texture;

		Sprite spr;
		spr.img.x = spr.img.y = 0;
		spr.img.w = texture.width;
		spr.img.h = texture.height;
		spr.pos = ivec2_0;
		render_state.sprite_buffers[getSpriteLayerForLevelLayer(editor_state.current_layer)].append(spr);

		boost::format status_text_fmt("TILE ID: %04X");
		status_text = str(status_text_fmt % editor_state.current_tile_id);
	}

	drawString(0, WINDOW_HEIGHT - ui_font.char_h, status_text, render_state.sprite_buffers[RenderState::LAYER_UI], ui_font, TextAlignment::left, color_white);
}
