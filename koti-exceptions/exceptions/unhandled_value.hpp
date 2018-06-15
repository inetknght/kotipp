#include <string_view>
#include <stdexcept>
#include <typeindex>
#include <optional>
#include <any>

namespace koti {
namespace exception {

class unhandled_value
: public std::logic_error
{
public:
    template <
        class unhandled_type
    > unhandled_value(
        std::string_view message
    )
    : std::logic_error{message}
    , type_index_(typeid(unhandled_type))
    , value_(std::nullopt)
    {
    }

    template <
        class unhandled_type
    > unhandled_value(
        std::string_view message,
        unhandled_type && t
    )
    : std::logic_error{message}
    , type_index_(typeid(unhandled_type))
    , value_(t)
    {
    }

    template <
        class unhandled_type
    > unhandled_value(
        unhandled_type && t
    )
    : std::logic_error{"unhandled value"}
    , type_index_(typeid(unhandled_type))
    , value_(t)
    {
    }

    const std::type_index &
    type_index(
    ) const
    {
        return type_index_;
    }

    const std::optional<std::any> &
    value(
    ) const
    {
        return value_;
    }

protected:
    std::type_index type_index_;
    std::optional<std::any> value_;
};

} // namespace exception
} // namespace koti
