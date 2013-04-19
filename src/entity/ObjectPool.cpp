#include "ObjectPool.hpp"
#include <cassert>

ObjectPool::ObjectPool(uint16_t pool_id, size_t object_size)
	: pool_id(pool_id), pool(object_size), first_free_index(SIZE_MAX)
{}

Handle ObjectPool::insert(const void* object) {
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

void ObjectPool::remove(const Handle h) {
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

void* ObjectPool::operator[] (const Handle h) {
	if (isValid(h)) {
		assert(roster[h.index].index < pool.size());
		return pool[roster[h.index].index];
	} else {
		return nullptr;
	}
}

const void* ObjectPool::operator[] (const Handle h) const {
	if (isValid(h)) {
		assert(roster[h.index].index < pool.size());
		return pool[roster[h.index].index];
	} else {
		return nullptr;
	}
}

/** Checks if object referenced by handle is still in the pool. */
bool ObjectPool::isValid(const Handle h) const {
	return h.type == pool_id && h.index < roster.size() && roster[h.index].generation == h.generation;
}

/** Creates a handle to the object currently at pool[index]. */
Handle ObjectPool::makeHandle(size_t index) const {
	if (index >= pool.size())
		return Handle();
	else
		return Handle(pool_id, index, roster[pool_indices[index]].generation);
}

/** Get index into pool for handle. */
size_t ObjectPool::getPoolIndex(const Handle h) const {
	if (isValid(h)) {
		return roster[h.index].index;
	} else {
		return SIZE_MAX;
	}
}

void ObjectPool::expand_roster() {
	const Handle new_entry(pool_id, first_free_index, 0);

	first_free_index = roster.size();
	roster.push_back(new_entry);

	assert(first_free_index < roster.size());
}
