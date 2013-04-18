#pragma once
#include "util/Position.hpp"
#include "util/vec2.hpp"

static const int WINDOW_WIDTH = 320;
static const int WINDOW_HEIGHT = 240;

struct Camera {
	Position pos;
	vec2 velocity;

	inline ivec2 transform(const ivec2& obj_pos) const {
		return (obj_pos - pos.integer()) + mivec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	}

	inline ivec2 inverse_transform(const ivec2 screen_pos) const {
		return pos.integer() + (screen_pos - mivec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));
	}
};