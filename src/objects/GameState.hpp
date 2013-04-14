#pragma once
#include "util/util.hpp"
#include "game_types.hpp"
#include <vector>

#include "Camera.hpp"
#include "BackgroundLayer.hpp"

struct GameState {
	RandomGenerator rng;
	InputButtons::Bitset input;
	int mouse_x, mouse_y;
	Camera camera;

	BackgroundLayer player_layer;

	double frametime_min, frametime_avg, frametime_max, fps;
};
