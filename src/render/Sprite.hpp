#pragma once

#include <array>
#include "util/vec2.hpp"
#include <cstdint>

struct Color {
	uint8_t r, g, b, a;
};

inline Color makeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	Color c = {r, g, b, a};
	return c;
}

static const Color color_white = {255, 255, 255, 255};

struct IntRect {
	int x, y;
	int w, h;
};

struct Sprite {
	ivec2 pos;
	IntRect img;
	Color color;

	Sprite() {
		const Color white = {255, 255, 255, 255};
		color = white;
	}

	void setImg(int x, int y, int w, int h) {
		img.x = x; img.y = y;
		img.w = w; img.h = h;
	}

	void setPos(int x_, int y_) {
		pos.x = x_; pos.y = y_;
	}

	void setPos(const vec2 v) {
		pos = vector_cast<int>(v);
	}
};

