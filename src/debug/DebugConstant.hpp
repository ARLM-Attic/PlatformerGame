#pragma once
#include <string>

std::string toString(float value);
void increment(float& value);
void decrement(float& value);
bool tryParse(float& value, const std::string& str);

std::string toString(bool value);
void increment(bool& value);
void decrement(bool& value);
bool tryParse(bool& value, const std::string& str);

struct DebugConstantBase;
extern DebugConstantBase* debug_constant_list_head;
extern DebugConstantBase* debug_constant_list_tail;

struct DebugConstantBase {
	const char* name;
	DebugConstantBase* prev;
	DebugConstantBase* next;

	DebugConstantBase(const char* name)
		: name(name), next(nullptr), prev(debug_constant_list_tail)
	{
		if (debug_constant_list_head == nullptr) {
			debug_constant_list_head = this;
		} else {
			debug_constant_list_tail->next = this;
		}
		debug_constant_list_tail = this;
	}

	virtual std::string toString_() const = 0;
	virtual void increment_() = 0;
	virtual void decrement_() = 0;
	virtual bool tryParse_(const std::string& str) = 0;
};

template <typename T>
struct DebugConstant : DebugConstantBase {
	T value;

	DebugConstant(T default_value, const char* name)
		: DebugConstantBase(name), value(default_value)
	{}

	operator T() {
		return value;
	}

	virtual std::string toString_() const {
		return toString(value);
	}

	virtual void increment_() {
		increment(value);
	}

	virtual void decrement_() {
		decrement(value);
	}

	virtual bool tryParse_(const std::string& str) {
		return tryParse(value, str);
	}
};
