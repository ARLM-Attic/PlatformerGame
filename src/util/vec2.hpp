#pragma once
#include <cmath>
#include <cassert>

template <typename T>
struct Vector2 {
	T x, y;

	inline T& operator[](int i) {
		if (i == 0) {
			return x;
		} else if (i == 1) {
			return y;
		} else {
			assert(false);
			return x;
		}
	}

	inline const T& operator[](int i) const {
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

typedef Vector2<float> vec2;
typedef Vector2<int> ivec2;

static const vec2 vec2_0 = {0.0f, 0.0f};
static const vec2 vec2_1 = {1.0f, 1.0f};
static const vec2 vec2_x = {1.0f, 0.0f};
static const vec2 vec2_y = {0.0f, 1.0f};

static const ivec2 ivec2_0 = {0, 0};
static const ivec2 ivec2_1 = {1, 1};
static const ivec2 ivec2_x = {1, 0};
static const ivec2 ivec2_y = {0, 1};

template <typename T>
inline Vector2<T> mVector2(T x, T y) {
	Vector2<T> tmp = {x, y};
	return tmp;
}

inline vec2 mvec2(float x, float y) {
	vec2 tmp = {x, y};
	return tmp;
}

inline ivec2 mivec2(int x, int y) {
	ivec2 tmp = {x, y};
	return tmp;
}

template <typename T>
inline bool operator ==(const Vector2<T> a, const Vector2<T> b) {
	return a.x == b.x && a.y == b.y;
}

template <typename T>
inline Vector2<T> operator +(const Vector2<T> a, const Vector2<T> b) {
	return mVector2<T>(a.x + b.x, a.y + b.y);
}

template <typename T>
inline Vector2<T> operator -(const Vector2<T> a, const Vector2<T> b) {
	return mVector2<T>(a.x - b.x, a.y - b.y);
}

template <typename T>
inline Vector2<T> operator *(const T s, const Vector2<T> v) {
	return mVector2<T>(s*v.x, s*v.y);
}

template <typename T>
inline Vector2<T> operator *(const Vector2<T> v, const T s) {
	return s * v;
}

template <typename T>
inline Vector2<T> operator *(const Vector2<T> a, const Vector2<T> b) {
	return mVector2<T>(a.x * b.x, a.y * b.y);
}

template <typename T>
inline Vector2<T> operator /(const T s, const Vector2<T> v) {
	return mVector2<T>(s/v.x, s/v.y);
}

template <typename T>
inline Vector2<T> operator /(const Vector2<T> v, const T s) {
	return mVector2<T>(v.x/s, v.y/s);
}

template <typename T>
inline Vector2<T> operator /(const Vector2<T> a, const Vector2<T> b) {
	return mVector2<T>(a.x / b.x, a.y / b.y);
}

template <typename T>
inline Vector2<T> operator %(const Vector2<T> v, const T s) {
	return mVector2<T>(v.x % s, v.y % s);
}

template <typename T>
inline Vector2<T> operator %(const Vector2<T> a, const Vector2<T> b) {
	return mVector2<T>(a.x % b.x, a.y % b.y);
}

template <typename T>
inline Vector2<T> operator -(const Vector2<T> v) {
	return mVector2<T>(-v.x, -v.y);
}

template <typename DSTT, typename SRCT>
inline Vector2<DSTT> vector_cast(const Vector2<SRCT> v) {
	return mVector2<DSTT>(DSTT(v.x), DSTT(v.y));
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
