#include <bitset>
#include <system_error>
#include <variant>

using bincode = std::bitset<16>;

struct assembler_error {
	std::errc ec{};
	std::string error_msg{};
};

using parser_string_result = std::variant<std::string, assembler_error>;
using bincode_result = std::variant<bincode, assembler_error>;