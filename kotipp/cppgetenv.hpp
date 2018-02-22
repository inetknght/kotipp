#pragma once

#include "optional.hpp"

namespace kotipp {

/**
 * @brief cppgetenv
 *  This is a wrapper around std::getenv.
 *
 *  Note that std::getenv is not thread safe with respect to setenv and friends.
 * @param name
 * @return std::optional<const char *>
 *  (const) value returned by std::getenv, or nullopt if not found
 */
std::optional<const char *> cppgetenv(const char * name);

/**
 * @brief cppgetenv
 *  This is a wrapper around std::getenv.
 *
 *  Note that std::getenv is not thread safe with respect to setenv and friends.
 * @param name
 * @param default_value
 *  if std::getenv(name) returned nullptr, then default_value is returned instead.
 * @return const char *
 */
const char * cppgetenv(const char * name, const char * default_value);

} // namespace kotipp
