#pragma once
#include "util/Position.hpp"
#include "util/vec2.hpp"

static const int WINDOW_WIDTH = 320;
static const int WINDOW_HEIGHT = 240;

struct Camera {
	ivec2 pos;

	inline vec2 transform(const ivec2& obj_pos) const {
		return vector_cast<float>(obj_pos - pos) + mvec2(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
	}

	inline ivec2 inverse_transform(const vec2 screen_pos) const {
		return pos + vector_cast<int>(screen_pos - mvec2(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f));
	}
};