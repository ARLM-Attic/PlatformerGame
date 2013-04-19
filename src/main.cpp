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
#include <sstream>
#include <ios>
#include <iomanip>
#include <numeric>
#include "util/util.hpp"
#include "util/Fixed.hpp"
#include "render/SpriteBuffer.hpp"
#include "util/vec2.hpp"
#include "render/graphics_init.hpp"
#include "render/texture.hpp"
#include "game_types.hpp"
#include "objects/GameState.hpp"
#include "RenderState.hpp"
#include "util/geometry.hpp"
#include "render/text.hpp"
#include "hud.hpp"
#include "tools/tools_main.hpp"
#include "Map.hpp"
#include "debug/DebugMenu.hpp"
#include "debug/Editor.hpp"

std::string formatFrametimeFloat(double x) {
	std::ostringstream ss;
	ss << std::fixed << std::setprecision(3) << x;
	return ss.str();
}

void drawFrametime(const GameState& game_state, SpriteBuffer& ui_buffer) {
	const std::string fps_text = "FPS: " + formatFrametimeFloat(game_state.fps);
	const std::string min_text = "MIN: " + formatFrametimeFloat(game_state.frametime_min * 1000.0f);
	const std::string avg_text = "AVG: " + formatFrametimeFloat(game_state.frametime_avg * 1000.0f);
	const std::string max_text = "MAX: " + formatFrametimeFloat(game_state.frametime_max * 1000.0f);

	drawString(WINDOW_WIDTH, 0*8, fps_text, ui_buffer, ui_font, TextAlignment::right, color_white);
	drawString(WINDOW_WIDTH, 1*8, min_text, ui_buffer, ui_font, TextAlignment::right, color_white);
	drawString(WINDOW_WIDTH, 2*8, avg_text, ui_buffer, ui_font, TextAlignment::right, color_white);
	drawString(WINDOW_WIDTH, 3*8, max_text, ui_buffer, ui_font, TextAlignment::right, color_white);
}

void drawScene(const GameState& game_state, RenderState& draw_state) {
	/* Draw scene */
	for (SpriteBuffer& buffer : draw_state.sprite_buffers) {
		buffer.clear();
	}

	game_state.player_layer.draw(draw_state.sprite_buffers[RenderState::LAYER_TILESET], game_state.camera);

	game_state.player.draw(draw_state.sprite_buffers[RenderState::LAYER_CHARACTER], game_state.camera);

	drawFrametime(game_state, draw_state.sprite_buffers[RenderState::LAYER_UI]);

	drawDebugMenu(draw_state.sprite_buffers[RenderState::LAYER_UI], ui_font);
	if (editorIsEnabled()) {
		editorDraw(game_state, draw_state);
	}

	/* Submit sprites */
	glClear(GL_COLOR_BUFFER_BIT);
	for (const SpriteBuffer& buffer : draw_state.sprite_buffers) {
		buffer.draw(draw_state.sprite_buffer_indices);
	}
}

void cameraSpring(const Player& player, Camera& camera) {
	vec2 displacement = vector_cast<float>(camera.pos.integer() - (player.pos.integer() + (player.size / 2) + mivec2(player.facing_direction * (WINDOW_WIDTH / 6), 0)));
	float k = 0.001f;
	float b = 2.f * std::sqrt(k);
	vec2 player_velocity = { player.move_velocity, player.jump_velocity };
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
	input.held.set(InputButtons::EDITOR_PICK_TILE, glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == GL_TRUE);
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

void updateScene(GameState& game_state) {
	readInput(game_state.input);

	if (editorIsEnabled()) {
		editorUpdate(game_state);
	} else {
		game_state.player.update(game_state);
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

	draw_state.sprite_buffers[RenderState::LAYER_TILESET].texture = loadTexture("tileset.png");

	draw_state.sprite_buffers[RenderState::LAYER_CHARACTER].texture = loadTexture("characters.png");
	draw_state.characters_sprdb.loadFromCsv("characters.csv");

	draw_state.sprite_buffers[RenderState::LAYER_UI].texture = loadTexture("ui_font.png");
	initUiFont();

	CHECK_GL_ERROR;

	///////////////////////////
	// Initialize game state //
	///////////////////////////
	GameState game_state;
	RandomGenerator& rng = game_state.rng;
	rng.seed(1235);

	{
		BackgroundLayer& l = game_state.player_layer;
		l.map = loadMap("tilemap.txt");
		l.tiles_per_row = 16;
		l.tile_size[0] = l.tile_size[1] = 16;
		l.position = mivec2(0, 0);
	}

	game_state.player.init(draw_state.characters_sprdb);
	game_state.player.pos = mPosition(96, 192);

	game_state.camera.pos = game_state.player.pos;
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
