#pragma once

#include <vector>
#include <cstdint>
#include "Handle.hpp"
#include "util/DynamicPoolAllocator.hpp"

/** Manages a pool of objects, providing persistent handles to them. */
struct ObjectPool {
	size_t first_free_index; // in roster
	uint16_t pool_id;

	// For used entries: .index is index into pool.
	// For free entries: .index is index of next free entry.
	std::vector<Handle> roster;

	DynamicPoolAllocator pool;
	std::vector<size_t> pool_indices;

	ObjectPool(uint16_t pool_id, size_t object_size);

	Handle insert(const void* object);
	void remove(const Handle h);

	void* operator[] (const Handle h);
	const void* operator[] (const Handle h) const;

	/** Checks if object referenced by handle is still in the pool. */
	bool isValid(const Handle h) const;

	/** Creates a handle to the object currently at pool[index]. */
	Handle makeHandle(size_t index) const;

	/** Get index into pool for handle. */
	size_t getPoolIndex(const Handle h) const;

private:
	void expand_roster();
};
