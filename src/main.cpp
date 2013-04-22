#include "gl/gl_1_5.h"
#include <GL/glfw.h>

#include "stb_image.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <cstdint>
#include <array>
#include <algorithm>
#include <string>
#include <numeric>
#include "util/util.hpp"
#include "util/Fixed.hpp"
#include "render/SpriteBuffer.hpp"
#include "util/vec2.hpp"
#include "render/graphics_init.hpp"
#include "render/texture.hpp"
#include "InputButtons.hpp"
#include "objects/GameState.hpp"
#include "RenderState.hpp"
#include "util/geometry.hpp"
#include "render/text.hpp"
#include "hud.hpp"
#include "tools/tools_main.hpp"
#include "Map.hpp"
#include "debug/DebugMenu.hpp"
#include "debug/Editor.hpp"
#include "entity/EntityFactory.hpp"
#include "entity/EntityIds.hpp"
#include "Context.hpp"

Context context;

void drawScene(const GameState& game_state, RenderState& draw_state) {
	/* Draw scene */
	draw_state.clearBuffers();

	if (editorIsEnabled()) {
		editorDraw(game_state, draw_state);
	} else {
		draw_state.drawSprites();
		draw_state.drawTileLayers();
		draw_state.drawFrametime();
	}

	drawDebugMenu(draw_state.sprite_buffers[RenderState::LAYER_UI], ui_font);

	draw_state.submitSprites();
}

void cameraSpring(const Handle player, Camera& camera) {
	const ivec2 player_pos = findInChain<PositionComponent>(player)->position;
	const ivec2 player_size = findInChain<BoundingRect>(player)->size;
	const CharacterMovement& movement = *findInChain<CharacterMovement>(player);

	vec2 displacement = vector_cast<float>(camera.pos.integer() - (player_pos + (player_size / 2) + mivec2(movement.facing_direction * (WINDOW_WIDTH / 6), 0)));
	float k = 0.001f;
	float b = 2.f * std::sqrt(k);
	vec2 player_velocity = { movement.move_velocity, movement.jump_velocity };
	vec2 force = -k * displacement - b * (camera.velocity - player_velocity);
	camera.velocity = camera.velocity + force;
	camera.pos = camera.pos + camera.velocity;
}

void readInput(InputButtons& input) {
	InputButtons::Bitset previous_held = input.held;

	input.held.set(InputButtons::LEFT, glfwGetKey(GLFW_KEY_LEFT) == GL_TRUE);
	input.held.set(InputButtons::RIGHT, glfwGetKey(GLFW_KEY_RIGHT) == GL_TRUE);
	input.held.set(InputButtons::UP, glfwGetKey(GLFW_KEY_UP) == GL_TRUE);
	input.held.set(InputButtons::DOWN, glfwGetKey(GLFW_KEY_DOWN) == GL_TRUE);
	input.held.set(InputButtons::SHOOT, glfwGetKey('X') == GL_TRUE);

	input.held.set(InputButtons::DEBUG_LEFT, glfwGetKey(GLFW_KEY_KP_4) == GL_TRUE);
	input.held.set(InputButtons::DEBUG_RIGHT, glfwGetKey(GLFW_KEY_KP_6) == GL_TRUE);
	input.held.set(InputButtons::DEBUG_UP, glfwGetKey(GLFW_KEY_KP_8) == GL_TRUE);
	input.held.set(InputButtons::DEBUG_DOWN, glfwGetKey(GLFW_KEY_KP_2) == GL_TRUE);
	input.held.set(InputButtons::DEBUG_ENTER, glfwGetKey(GLFW_KEY_KP_ENTER) == GL_TRUE);

	input.held.set(InputButtons::EDITOR_TOGGLE, glfwGetKey('`') == GL_TRUE);
	input.held.set(InputButtons::EDITOR_CHOOSE_TILE, glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == GL_TRUE);
	input.held.set(InputButtons::EDITOR_PICK_TILE, glfwGetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE) == GL_TRUE);
	input.held.set(InputButtons::EDITOR_PLACE_TILE, glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GL_TRUE);
	input.held.set(InputButtons::EDITOR_SCROLL_LEFT, glfwGetKey(GLFW_KEY_LEFT) == GL_TRUE);
	input.held.set(InputButtons::EDITOR_SCROLL_RIGHT, glfwGetKey(GLFW_KEY_RIGHT) == GL_TRUE);
	input.held.set(InputButtons::EDITOR_SCROLL_UP, glfwGetKey(GLFW_KEY_UP) == GL_TRUE);
	input.held.set(InputButtons::EDITOR_SCROLL_DOWN, glfwGetKey(GLFW_KEY_DOWN) == GL_TRUE);

	input.pressed = ~previous_held & input.held;
	input.released = previous_held & ~input.held;

	glfwGetMousePos(&input.mouse_pos.x, &input.mouse_pos.y);
	input.mouse_pos.x = clamp(0, input.mouse_pos.x, WINDOW_WIDTH-1);
	input.mouse_pos.y = clamp(0, input.mouse_pos.y, WINDOW_HEIGHT-1);
}

void InputButtons::warpMouse(ivec2 new_pos) {
	glfwSetMousePos(new_pos.x, new_pos.y);
}

void updateScene(GameState& game_state) {
	readInput(game_state.input);

	if (editorIsEnabled()) {
		editorUpdate(game_state);
	} else {
		for (CharacterMovement& movement : game_state.component_manager.component_pool_CharacterMovement) {
			movement.update(game_state.input, game_state.level_layers[GameState::LAYER_FOREGROUND]);
		}
		cameraSpring(game_state.player, game_state.camera);
	}

	updateDebugMenu(game_state.input);
}

int main(int argc, const char* argv[]) {

#ifndef NDEBUG
	if (argc >= 2 && strcmp(argv[1], "-t") == 0) {
		return tools_main(argc - 2, argv + 2);
	}
#endif

	if (!initWindow(WINDOW_WIDTH, WINDOW_HEIGHT)) {
		std::cerr << "Failed to initialize window.\n";
		return 1;
	}

	{

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_MODELVIEW);
	GLfloat view_matrix[16] = {
		2.0f/WINDOW_WIDTH,  0.0f,               0.0f, -1.0f,
		0.0f,              -2.0/WINDOW_HEIGHT,  0.0f, 1.0f,
		0.0f,               0.0f,               1.0f, 0.0f,
		0.0f,               0.0f,               0.0f, 1.0f
	};
	glLoadTransposeMatrixf(view_matrix);

	CHECK_GL_ERROR;

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	glClearColor(0.08f, 0.08f, 0.08f, 1.0f);

	CHECK_GL_ERROR;

	RenderState draw_state;
	CHECK_GL_ERROR;

	draw_state.sprite_buffers[RenderState::LAYER_TILES_FG].texture = loadTexture("tileset.png");

	draw_state.sprite_buffers[RenderState::LAYER_CHARACTER].texture = loadTexture("characters.png");
	draw_state.characters_sprdb.loadFromCsv("characters.csv");

	draw_state.sprite_buffers[RenderState::LAYER_UI].texture = loadTexture("ui_font.png");
	initUiFont();

	CHECK_GL_ERROR;

	///////////////////////////
	// Initialize game state //
	///////////////////////////
	GameState game_state;

	context.game_state = &game_state;
	context.render_state = &draw_state;
	context.component_mgr = &game_state.component_manager;

	game_state.rng.seed(1235);

	{
		BackgroundLayer& l = game_state.level_layers[GameState::LAYER_FOREGROUND];
		l.map = loadMap("tilemap.txt");
		l.tile_size.x = l.tile_size.y = 16;
		const TextureInfo& texture = draw_state.sprite_buffers[RenderState::LAYER_TILES_FG].texture;
		l.tiles_per_row = texture.width / l.tile_size.x;
		int tile_rows = texture.height / l.tile_size.y;
		l.num_tiles = tile_rows * l.tiles_per_row;
		l.position = mivec2(0, 0);
	}

	game_state.player = createEntity(EntityId::Player);

	game_state.camera.pos = mPosition(findInChain<PositionComponent>(game_state.player)->position);
	game_state.camera.velocity = vec2_0;
	
	editorInit();

	////////////////////
	// Main game loop //
	////////////////////
	std::array<double, 60> frametimes;
	unsigned int frametimes_pos = 0;
	frametimes.fill(1.0 / 60.0);

	bool running = true;
	double update_time = 0.0;
	static const double TIMESTEP = 1.0 / 60.0;
	double last_time = glfwGetTime();
	while (running) {
		double cur_time = glfwGetTime();
		const double frame_time = cur_time - last_time;
		update_time += frame_time;
		last_time = cur_time;

		frametimes[frametimes_pos] = frame_time;
		if (++frametimes_pos >= frametimes.size()) frametimes_pos = 0;

		while (update_time > 0.0) {
			updateScene(game_state);
			update_time -= TIMESTEP;
		}

		auto frametimes_minmax = std::minmax_element(frametimes.cbegin(), frametimes.cend());
		game_state.frametime_min = *frametimes_minmax.first;
		game_state.frametime_max = *frametimes_minmax.second;
		game_state.frametime_avg = std::accumulate(frametimes.cbegin(), frametimes.cend(), 0.0) / frametimes.size();
		game_state.fps = 1.f / game_state.frametime_avg;

		running = running && glfwGetWindowParam(GLFW_OPENED);
		running = running && glfwGetKey(GLFW_KEY_ESC) == GL_FALSE;
		if (!running) {
			continue;
		}

		drawScene(game_state, draw_state);
		glfwSwapBuffers();
		CHECK_GL_ERROR;
	}

	}

	glfwCloseWindow();
	glfwTerminate();
}
