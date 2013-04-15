#pragma once
#include <cmath>
#include <cassert>

struct vec2 {
	float x, y;

	inline float& operator[](int i) {
		if (i == 0) {
			return x;
		} else if (i == 1) {
			return y;
		} else {
			assert(false);
			return x;
		}
	}

	inline const float& operator[](int i) const {
		if (i == 0) {
			return x;
		} else if (i == 1) {
			return y;
		} else {
			assert(false);
			return x;
		}
	}
};

static const vec2 vec2_0 = {0.0f, 0.0f};
static const vec2 vec2_1 = {1.0f, 1.0f};
static const vec2 vec2_x = {1.0f, 0.0f};
static const vec2 vec2_y = {0.0f, 1.0f};

inline vec2 mvec2(float x, float y) {
	vec2 tmp = {x, y};
	return tmp;
}

inline bool operator ==(const vec2 a, const vec2 b) {
	return a.x == b.x && a.y == b.y;
}

inline vec2 operator +(const vec2 a, const vec2 b) {
	return mvec2(a.x + b.x, a.y + b.y);
}

inline vec2 operator -(const vec2 a, const vec2 b) {
	return mvec2(a.x - b.x, a.y - b.y);
}

inline vec2 operator *(const float s, const vec2 v) {
	return mvec2(s*v.x, s*v.y);
}

inline vec2 operator *(const vec2 v, const float s) {
	return s * v;
}

inline vec2 operator /(const float s, const vec2 v) {
	return mvec2(s/v.x, s/v.y);
}

inline vec2 operator /(const vec2 v, const float s) {
	return mvec2(v.x/s, v.y/s);
}

inline vec2 operator -(const vec2 v) {
	return mvec2(-v.x, -v.y);
}

inline float dot(const vec2 a, const vec2 b) {
	return a.x*b.x + a.y*b.y;
}

inline float length_sqr(const vec2 v) {
	return dot(v, v);
}

inline vec2 normalized(const vec2 v) {
	return v / std::sqrt(length_sqr(v));
}

static const vec2 complex_1 = {1.0f, 0.0f};

inline vec2 complex_mul(const vec2 a, const vec2 b) {
	return mvec2(a.x*b.x - a.y*b.y, a.y*b.x + a.x*b.y);
}

inline vec2 complex_from_angle(float radians) {
	return mvec2(std::cos(radians), std::sin(radians));
}

inline vec2 complex_conjugate(const vec2 c) {
	return mvec2(c.x, -c.y);
}

inline bool linearly_dependent(const vec2 a, const vec2 b) {
	return a.x*b.y - a.y*b.x < 1e-4f;
}

inline vec2 lerp(const vec2 a, const vec2 b, float t) {
	return (1.0f - t)*a + t*b;
}

inline vec2 nlerp(const vec2 a, const vec2 b, float t) {
	return normalized(lerp(a, b, t));
}

vec2 slerp(vec2 a, vec2 b, float t);
vec2 rotateTowards(vec2 a, vec2 b, float max_angle);
