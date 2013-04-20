#pragma once
#include "Fixed.hpp"
#include "vec2.hpp"
#include <cassert>

typedef fixed24_8 PositionFixed;

struct Position {
	PositionFixed x, y;

	inline PositionFixed& operator[](int i) {
		if (i == 0) {
			return x;
		} else if (i == 1) {
			return y;
		} else {
			assert(false);
			return x;
		}
	}

	inline const PositionFixed& operator[](int i) const {
		if (i == 0) {
			return x;
		} else if (i == 1) {
			return y;
		} else {
			assert(false);
			return x;
		}
	}

	ivec2 integer() const {
		return mivec2(x.integer(), y.integer());
	}
};

inline Position mPosition(const PositionFixed x, const PositionFixed y) {
	Position pos = {x, y};
	return pos;
}

inline Position mPosition(const ivec2 v) {
	Position pos = {v.x, v.y};
	return pos;
}

inline vec2 operator -(const Position a, const Position b) {
	return mvec2((a.x - b.x).toFloat(), (a.y - b.y).toFloat());
}

inline Position& operator +=(Position& p, const vec2 v) {
	p.x += PositionFixed(v.x);
	p.y += PositionFixed(v.y);
	return p;
}

inline Position& operator -=(Position& p, const vec2 v) {
	p.x -= PositionFixed(v.x);
	p.y -= PositionFixed(v.y);
	return p;
}

inline Position operator +(const Position p, const vec2 v) {
	return mPosition(p.x + PositionFixed(v.x), p.y + PositionFixed(v.y));
}

inline Position operator -(const Position p, const vec2 v) {
	return mPosition(p.x - PositionFixed(v.x), p.y - PositionFixed(v.y));
}
