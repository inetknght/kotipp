
#include "cppgetenv.hpp"

#include <cstdlib>

namespace kotipp {

std::optional<const char *> cppgetenv(const char * name) {
	const char * result = std::getenv(name);
	if ( nullptr == result ) {
		return std::nullopt;
	}
	return {result};
}

const char * cppgetenv(const char * name, const char * default_value) {
	const char * result = std::getenv(name);
	if ( nullptr == result ) {
		return default_value;
	}
	return result;
}

} // namespace kotipp
