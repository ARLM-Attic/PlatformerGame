#pragma once

#include <vector>
#include <type_traits>
#include <cassert>
#include <cstdint>
#include "Handle.hpp"
#include "util/DynamicPoolAllocator.hpp"

/** Manages a pool of objects, providing persistent handles to them. */
struct ObjectPoolBase {
	size_t first_free_index; // in roster
	uint16_t pool_id;

	// For used entries: .index is index into pool.
	// For free entries: .index is index of next free entry.
	std::vector<Handle> roster;

	DynamicPoolAllocator pool;
	std::vector<size_t> pool_indices;

	ObjectPoolBase(uint16_t pool_id, size_t object_size)
		: pool_id(pool_id), pool(object_size), first_free_index(SIZE_MAX)
	{}

	Handle insert(const void* object) {
		// Expand roster if we're out of entries
		if (first_free_index >= roster.size()) {
			expand_roster();
		}

		// Pop head off of free list
		const size_t roster_index = first_free_index;
		first_free_index = roster[roster_index].index;

		// Point roster entry to right place and insert object
		roster[roster_index].index = pool.size();
		if (object != nullptr) {
			pool.push_back(object);
		} else {
			pool.push_back();
		}
		pool_indices.push_back(roster_index);

		return Handle(pool_id, roster_index, roster[roster_index].generation);
	}

	void remove(const Handle h) {
		if (!isValid(h))
			return;

		// Indices for object being removed
		const size_t roster_index = h.index;
		const size_t pool_index = roster[roster_index].index;

		// Indices for object being moved into its place
		const size_t moved_roster_index = pool_indices.back();
		const size_t moved_pool_index = pool.size() - 1;
		assert(roster[moved_roster_index].index == moved_pool_index);

		// Move last element in place of the removed one, updating roster
		if (pool_index != moved_pool_index) {
			roster[moved_roster_index].index = pool_index;
			pool.copy(moved_pool_index, pool_index);
			pool_indices[pool_index] = pool_indices[moved_pool_index];
		}
		pool.pop_back();
		pool_indices.pop_back();

		// Increment generation of removed roster entry and add it to free list
		++roster[roster_index].generation;
		roster[roster_index].index = first_free_index;
		first_free_index = roster_index;
	}

	void* operator[] (const Handle h) {
		if (isValid(h)) {
			assert(roster[h.index].index < pool.size());
			return pool[roster[h.index].index];
		} else {
			return nullptr;
		}
	}

	const void* operator[] (const Handle h) const {
		if (isValid(h)) {
			assert(roster[h.index].index < pool.size());
			return pool[roster[h.index].index];
		} else {
			return nullptr;
		}
	}

	/** Checks if object referenced by handle is still in the pool. */
	bool isValid(const Handle h) const {
		return h.pool == pool_id && h.index < roster.size() && roster[h.index].generation == h.generation;
	}

	/** Creates a handle to the object currently at pool[index]. */
	Handle makeHandle(size_t index) const {
		if (index >= pool.size())
			return Handle();
		else
			return Handle(pool_id, index, roster[pool_indices[index]].generation);
	}

	/** Get index into pool for handle. */
	size_t getPoolIndex(const Handle h) const {
		if (isValid(h)) {
			return roster[h.index].index;
		} else {
			return SIZE_MAX;
		}
	}

private:
	void expand_roster() {
		const Handle new_entry(pool_id, first_free_index, 0);

		first_free_index = roster.size();
		roster.push_back(new_entry);

		assert(first_free_index < roster.size());
	}
};

template <typename T>
struct ObjectPool : ObjectPoolBase {
	static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");

	ObjectPool() : ObjectPoolBase(ObjectTags<T>::id, sizeof(T)) {}

	Handle insert() {
		return ObjectPoolBase::insert(nullptr);
	}

	Handle insert(const T& object) {
		return ObjectPoolBase::insert(&object);
	}

	T* operator[] (const Handle h) {
		return static_cast<T*>(ObjectPoolBase::operator[](h));
	}

	const T* operator[] (const Handle h) const {
		return static_cast<const T*>(ObjectPoolBase::operator[](h));
	}
};

#if 0
template<typename T> struct ObjectTags;

template<> struct ObjectTags<float> {
	static const uint16_t id = 1;
};

inline void test_ObjectPool() {
	ObjectPool<float> objp;
	const auto& objp_c = objp;

	Handle h1 = objp.insert(42.0f);
	Handle h2 = objp.insert(1000.0f);

	float* f1 = objp[h1];
	assert(f1 && *f1 == 42.0f);
	const float* f2 = objp_c[h2];
	assert(f2 && *f2 == 1000.0f);

	objp.remove(h2);

	const float* f3 = objp_c[h2];
	assert(!f3);

	objp.remove(h1);

	Handle h3 = objp.insert(112233.0f);

	objp.remove(h1);

	const float* f4 = objp[h3];
	assert(f4 && *f4 == 112233.0f);

	objp.remove(h3);

	assert(objp.pool.size() == 0);
}
#endif
