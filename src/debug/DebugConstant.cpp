#include "DebugConstant.hpp"
#include <sstream>
#include <iomanip>

std::string toString(float value) {
	std::ostringstream sstr;
	sstr << std::setiosflags(std::ios::fixed) << std::setprecision(3) << value;
	return sstr.str();
}

void increment(float& value) {
	value *= 1.01f;
}

void decrement(float& value) {
	value /= 1.01f;
}

bool tryParse(float& value, const std::string& str) {
	try {
		value = std::stof(str);
		return true;
	} catch (std::invalid_argument&) {
		return false;
	}
}
