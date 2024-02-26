#pragma once
#include <cassert>
#include <string>
#include <bitset>
#include <algorithm>
#include <charconv>
#include <cstdint>
#include "code_module.h"
#include <optional>
#include <variant>
#include "assembler_defines.h"


bincode_result get_a_instruction_bincode(std::string instruction) {

	instruction.erase(0, 1);

	uint16_t value{};
	int base = 10;
	if (instruction[0] == '0' and instruction[1] == 'x') {
		base = 16;
		instruction.erase(0, 2);
	}
	else if (instruction[0] == '0' and instruction[1] == 'b') {
		base = 2;
		instruction.erase(0, 2);
	}

	auto parse_into_int_result = std::from_chars(instruction.data(), instruction.data() + instruction.size(), value, base);

	if (value > pow(15, 2)) {
		return assembler_error{ .ec = std::errc::value_too_large, .error_msg = "value in a instruction is larger than 15 bits" };
	}
	if ((int)parse_into_int_result.ec == 0) {
		return bincode{ get_a_instruction_bincode(value) };
	}
	else {
		return assembler_error{ .ec = std::errc::invalid_argument, .error_msg = "couldnt parse a instruction into valid int" };
	}
}

parser_string_result get_dest_string(std::string instruction) {
	size_t pos_of_equ = instruction.find('=');
	if (pos_of_equ == std::string::npos) {
		return assembler_error{ .ec = std::errc::invalid_argument, .error_msg = " '=' not found in c instruction" };
	}

	return std::string{ instruction.data(), pos_of_equ };

}

parser_string_result get_comp_string(std::string instruction) {
	size_t pos_of_equ = instruction.find('=');
	size_t pos_of_semicolon = instruction.find(';');

	if (pos_of_equ == std::string::npos) {
		return assembler_error{ .ec = std::errc::invalid_argument, .error_msg = " '=' not found in c instruction" };
	}
	
	if ( pos_of_semicolon != std::string::npos and pos_of_semicolon < pos_of_equ) {
		return assembler_error{ .ec = std::errc::invalid_argument, .error_msg = " ; is placed before = in c instruction while parsing comp string" };
	}

	std::string comp_part{  };

	if (pos_of_semicolon != std::string::npos) {
		comp_part = std::string{ instruction.data() + pos_of_equ + 1,  pos_of_semicolon - pos_of_equ - 1 };
	}
	else {
		comp_part = std::string{ instruction.data() + pos_of_equ + 1, instruction.length() - pos_of_equ - 1 };
	}

	return comp_part;
}

parser_string_result get_jmp_string(std::string instruction) {
	size_t pos_of_semicolon = instruction.find(';');
	if (pos_of_semicolon == std::string::npos) {
		return assembler_error{ .ec = std::errc::invalid_argument, .error_msg = "could not find ; in c instruction while parsing jmp string " };
	}
	else if (pos_of_semicolon == instruction.length() - 1) {
		return assembler_error{ .ec = std::errc::invalid_argument, .error_msg = "; is the last char in c instruction while parsing jmp string" };
	}
	return std::string{ instruction.data() + pos_of_semicolon + 1, instruction.length() - 1 };
}

std::bitset<16> get_c_instruction_bincode(std::string instruction) {

	auto dest_part = get_dest_string(instruction);
	auto comp_part = get_comp_string(instruction);

	return std::bitset<16>{};

}
std::bitset<16> instruction_string_to_bin(std::string instruction) {
	instruction.erase(std::remove(instruction.begin(), instruction.end(), ' '), instruction.end());
	if (instruction[0] == '@') {
		//return get_a_instruction_bincode(instruction).value();
	}
	else {
		return get_c_instruction_bincode(instruction);
	}
}