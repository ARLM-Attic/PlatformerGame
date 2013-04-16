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

std::string formatFrametimeFloat(double x) {
	std::ostringstream ss;
	ss << std::fixed << std::setprecision(3) << x;
	return ss.str();
}

void drawScene(const GameState& game_state, RenderState& draw_state) {
	/* Draw scene */
	draw_state.tileset_buffer.clear();
	draw_state.characters_buffer.clear();
	draw_state.ui_buffer.clear();

	game_state.player_layer.draw(draw_state.tileset_buffer, game_state.camera);

	game_state.player.draw(draw_state.characters_buffer, game_state.camera);

	/* Draw FPS */
	{
		const std::string fps_text = "FPS: " + formatFrametimeFloat(game_state.fps);
		const std::string min_text = "MIN: " + formatFrametimeFloat(game_state.frametime_min * 1000.0f);
		const std::string avg_text = "AVG: " + formatFrametimeFloat(game_state.frametime_avg * 1000.0f);
		const std::string max_text = "MAX: " + formatFrametimeFloat(game_state.frametime_max * 1000.0f);

		drawString(WINDOW_WIDTH, 0*8, fps_text, draw_state.ui_buffer, ui_font, TextAlignment::right, color_white);
		drawString(WINDOW_WIDTH, 1*8, min_text, draw_state.ui_buffer, ui_font, TextAlignment::right, color_white);
		drawString(WINDOW_WIDTH, 2*8, avg_text, draw_state.ui_buffer, ui_font, TextAlignment::right, color_white);
		drawString(WINDOW_WIDTH, 3*8, max_text, draw_state.ui_buffer, ui_font, TextAlignment::right, color_white);
	}

	/* Submit sprites */
	glClear(GL_COLOR_BUFFER_BIT);
	draw_state.tileset_buffer.draw(draw_state.sprite_buffer_indices);
	draw_state.characters_buffer.draw(draw_state.sprite_buffer_indices);
	draw_state.ui_buffer.draw(draw_state.sprite_buffer_indices);
}

void updateScene(GameState& game_state) {
	InputButtons::Bitset& input = game_state.input;
	input.set(InputButtons::LEFT, glfwGetKey(GLFW_KEY_LEFT) == GL_TRUE);
	input.set(InputButtons::RIGHT, glfwGetKey(GLFW_KEY_RIGHT) == GL_TRUE);
	input.set(InputButtons::UP, glfwGetKey(GLFW_KEY_UP) || glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT));
	input.set(InputButtons::DOWN, glfwGetKey(GLFW_KEY_DOWN) || glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT));
	input.set(InputButtons::SHOOT, glfwGetKey('X') || glfwGetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE));
	//input.set(InputButtons::RIGHT, true);
	//glfwGetMousePos(&game_state.mouse_x, &game_state.mouse_y);
	//game_state.mouse_x = clamp(0, game_state.mouse_x, WINDOW_WIDTH-1);
	//game_state.mouse_y = clamp(0, game_state.mouse_y, WINDOW_HEIGHT-1);
	//glfwSetMousePos(game_state.mouse_x, game_state.mouse_y);

	game_state.player.update(game_state, input);

	vec2 displacement = vector_cast<float>(game_state.camera.pos.integer() - (game_state.player.pos.integer() + (game_state.player.size / 2) + mivec2(game_state.player.facing_direction * 80, 0)));
	float k = 0.035f;
	vec2 b = { 0.8f, 0.6f };
	vec2 mass = { 1.0f, 0.2f };
	vec2 player_velocity = { game_state.player.move_velocity, game_state.player.jump_velocity * 0.75f };
	vec2 force = -k * displacement - b * (game_state.camera.velocity - player_velocity);
	game_state.camera.velocity = game_state.camera.velocity + force * mass;
	game_state.camera.pos = game_state.camera.pos + game_state.camera.velocity;//;
	
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

	draw_state.tileset_buffer.texture = loadTexture("tileset.png");

	draw_state.characters_buffer.texture = loadTexture("characters.png");
	draw_state.characters_sprdb.loadFromCsv("characters.csv");

	draw_state.ui_buffer.texture = loadTexture("ui_font.png");
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

		drawScene(game_state, draw_state);

		glfwSwapBuffers();
		running = running && glfwGetWindowParam(GLFW_OPENED);
		running = running && glfwGetKey(GLFW_KEY_ESC) == GL_FALSE;

		CHECK_GL_ERROR;
	}

	}

	glfwCloseWindow();
	glfwTerminate();
}
