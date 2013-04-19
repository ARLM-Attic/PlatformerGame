#pragma once

#include <cstdint>

struct Handle {
	uint16_t type;
	uint16_t index;
	uint32_t generation;

	Handle()
		: type(-1), index(-1), generation(-1)
	{}

	Handle(uint16_t type, uint16_t index, uint32_t generation)
		: type(type), index(index), generation(generation)
	{}

	bool operator ==(const Handle& o) const {
		return type == o.type && index == o.index && generation == o.generation;
	}

	bool operator !=(const Handle& o) const {
		return !(*this == o);
	}

	bool isNull() const {
		return index == -1;
	}
};
