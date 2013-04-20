#pragma once
#include "util/util.hpp"
#include "game_types.hpp"
#include <vector>

#include "Camera.hpp"
#include "BackgroundLayer.hpp"
#include "entity/ComponentManager.hpp"

struct GameState {
	RandomGenerator rng;
	InputButtons input;
	Camera camera;

	BackgroundLayer player_layer;

	Handle player;
	ComponentManager component_manager;

	double frametime_min, frametime_avg, frametime_max, fps;
};
