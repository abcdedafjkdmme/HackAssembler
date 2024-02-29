#include <bitset>
#include <system_error>
#include <variant>


struct assembler_error {
	std::errc ec{};
	std::string error_msg{};
};

using bincode = std::bitset<16>;
using comp_bincode = std::bitset<7>;
using dest_bincode = std::bitset<3>;
using jmp_bincode = std::bitset<3>;
using parser_string_result = std::variant<std::string, assembler_error>;
using bincode_result = std::variant<bincode, assembler_error>;
