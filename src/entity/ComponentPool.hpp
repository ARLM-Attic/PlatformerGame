#pragma once

#include "ObjectPool.hpp"
#include "Component.hpp"
#include <type_traits>
#include <cassert>

template <typename T>
struct ComponentPool : ObjectPool {
	static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");

	ComponentPool() : ObjectPool(T::COMPONENT_ID, sizeof(T)) {}

	Handle insert() {
		return insert(nullptr);
	}

	Handle insert(const T& object) {
		return insert(&object);
	}

	Handle insert(const T* object) {
		Handle h = ObjectPool::insert(object);
		Component* c = (*this)[h];
		c->self = c->next = h;
		return h;
	}

	T* operator[] (const Handle h) {
		return static_cast<T*>(ObjectPool::operator[](h));
	}

	const T* operator[] (const Handle h) const {
		return static_cast<const T*>(ObjectPool::operator[](h));
	}

	T* begin() { return static_cast<T*>(pool.begin()); }
	T* end() { return static_cast<T*>(pool.end()); }
	const T* begin() const { return static_cast<const T*>(pool.begin()); }
	const T* end() const { return static_cast<const T*>(pool.end()); }
};

#if 0
struct FloatComponent : Component {
	static const uint16_t COMPONENT_ID = 10000;

	float value;
	explicit FloatComponent(float value) : value(value) {}
};

inline void test_ComponentPool() {
	ComponentPool<FloatComponent> objp;
	const auto& objp_c = objp;

	Handle h1 = objp.insert(FloatComponent(42.0f));
	Handle h2 = objp.insert(FloatComponent(1000.0f));

	FloatComponent* f1 = objp[h1];
	assert(f1 && f1->value == 42.0f);
	const FloatComponent* f2 = objp_c[h2];
	assert(f2 && f2->value == 1000.0f);

	objp.remove(h2);

	const FloatComponent* f3 = objp_c[h2];
	assert(!f3);

	objp.remove(h1);

	Handle h3 = objp.insert(FloatComponent(112233.0f));

	objp.remove(h1);

	const FloatComponent* f4 = objp[h3];
	assert(f4 && f4->value == 112233.0f);

	objp.remove(h3);

	assert(objp.pool.size() == 0);
}
#endif