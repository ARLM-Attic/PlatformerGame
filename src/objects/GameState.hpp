#pragma once
#include "util/util.hpp"
#include "game_types.hpp"
#include <vector>

#include "Camera.hpp"
#include "Map.hpp"

struct GameState {
	RandomGenerator rng;
	InputButtons::Bitset input;
	int mouse_x, mouse_y;
	Camera camera;

	Map map;

	double frametime_min, frametime_avg, frametime_max, fps;
};
