#pragma once
#include "util/util.hpp"
#include "InputButtons.hpp"
#include <vector>

#include "Camera.hpp"
#include "BackgroundLayer.hpp"
#include "entity/ComponentManager.hpp"

struct GameState {
	enum LevelLayer {
		LAYER_FOREGROUND,
		LAYER_MAX
	};

	RandomGenerator rng;
	InputButtons input;
	Camera camera;

	std::array<BackgroundLayer, LAYER_MAX> level_layers;

	Handle player;
	ComponentManager component_manager;

	double frametime_min, frametime_avg, frametime_max, fps;
};
