#include "DynamicPoolAllocator.hpp"
#include <cstring>
#include <cassert>

DynamicPoolAllocator::DynamicPoolAllocator(size_t object_size)
	: object_size(object_size),
	data_begin(nullptr), data_end(nullptr),
	data_alloc_end(nullptr)
{}

DynamicPoolAllocator::DynamicPoolAllocator(const DynamicPoolAllocator& o)
	: object_size(o.object_size),
	data_begin(nullptr), data_end(nullptr),
	data_alloc_end(nullptr)
{
	*this = o;
}

DynamicPoolAllocator::~DynamicPoolAllocator() {
	delete[] data_begin;
}

DynamicPoolAllocator& DynamicPoolAllocator::operator =(const DynamicPoolAllocator& o) {
	delete[] data_begin;
	data_begin = new uint8_t[o.data_alloc_end - o.data_begin];
	data_end = data_begin + (o.data_end - o.data_begin);
	data_alloc_end = data_begin + (o.data_alloc_end - o.data_begin);
	std::memcpy(data_begin, o.data_begin, o.data_end - o.data_begin);

	return *this;
}

size_t DynamicPoolAllocator::getObjectSize() const {
	return object_size;
}

size_t DynamicPoolAllocator::size() const {
	return (data_end - data_begin) / object_size;
}

size_t DynamicPoolAllocator::capacity() const {
	return (data_alloc_end - data_begin) / object_size;
}

void DynamicPoolAllocator::reserve(size_t num) {
	if (num * object_size < (size_t)(data_alloc_end - data_begin)) {
		expand(num * object_size);
	}
}

void DynamicPoolAllocator::push_back() {
	if (data_end == data_alloc_end) {
		expand();
	}

	std::memset(data_end, 0x55, object_size);
	data_end += object_size;
}

void DynamicPoolAllocator::push_back(const void* src_data) {
	if (data_end == data_alloc_end) {
		expand();
	}

	std::memcpy(data_end, src_data, object_size);
	data_end += object_size;
}

void DynamicPoolAllocator::pop_back() {
	assert(data_end != data_begin);
	data_end -= object_size;
}

void DynamicPoolAllocator::copy(size_t from, size_t to) {
	assert(from != to);
	std::memcpy(data_begin + from * object_size, data_begin + to * object_size, object_size);
}

void* DynamicPoolAllocator::operator [](size_t i) {
	uint8_t* p = data_begin + i*object_size;
	assert(p < data_end);
	return p;
}

const void* DynamicPoolAllocator::operator [](size_t i) const {
	const uint8_t* p = data_begin + i*object_size;
	assert(p < data_end);
	return p;
}

void DynamicPoolAllocator::expand(size_t new_capacity_bytes) {
	assert(new_capacity_bytes > (size_t)(data_alloc_end - data_begin));
	uint8_t* new_begin = new uint8_t[new_capacity_bytes];
	uint8_t* new_end = new_begin + (data_end - data_begin);

	std::memcpy(new_begin, data_begin, data_end - data_begin);
	delete[] data_begin;

	data_begin = new_begin;
	data_end = new_end;
	data_alloc_end = new_begin + new_capacity_bytes;
}

void DynamicPoolAllocator::expand() {
	if (data_alloc_end == data_begin) {
		expand(4 * object_size);
	} else {
		expand((data_alloc_end - data_begin) * 2);
	}
}
