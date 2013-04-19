#pragma once

#include <cstdint>

struct Handle {
	uint16_t pool;
	uint16_t index;
	uint32_t generation;

	Handle()
		: pool(-1), index(-1), generation(-1)
	{}

	Handle(uint16_t pool, uint16_t index, uint32_t generation)
		: pool(pool), index(index), generation(generation)
	{}

	bool operator ==(const Handle& o) const {
		return pool == o.pool && index == o.index && generation == o.generation;
	}

	bool operator !=(const Handle& o) const {
		return !(*this == o);
	}

	bool isNull() const {
		return index == -1;
	}
};
