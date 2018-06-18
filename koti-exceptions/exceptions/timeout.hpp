#include <string>
#include <string_view>
#include <stdexcept>
#include <typeindex>
#include <optional>
#include <any>

namespace koti {
namespace exception {

class timeout
: public std::runtime_error
{
public:
    timeout(
        std::string_view message
    )
        : std::runtime_error{std::string{message}}
    {
    }
};

} // namespace exception
} // namespace koti
