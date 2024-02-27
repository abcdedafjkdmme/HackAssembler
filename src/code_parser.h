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
#include <map>
#include <format>

bool does_instruction_have_dest_part(std::string instruction) {
	return instruction.find('=') != std::string::npos;
}

bool does_instruction_have_comp_part(std::string instruction) {
	return true;
}

bool does_instruction_have_jmp_part(std::string instruction) {
	return instruction.find(';') != std::string::npos;
}

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

	if (pos_of_equ == std::string::npos and instruction[0] == '0') {
		return std::string{ "0" };

	}
	if (pos_of_equ == std::string::npos and instruction[0] != '0') {
		return assembler_error{ .ec = std::errc::invalid_argument, .error_msg = "'=' not found in c instruction and the first char isnt 0 ,while parsing comp string" };
	}

	if ((pos_of_semicolon != std::string::npos) and (pos_of_semicolon < pos_of_equ)) {
		return assembler_error{ .ec = std::errc::invalid_argument, .error_msg = " ; is placed before = in c instruction ,while parsing comp string" };
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
		return assembler_error{ .ec = std::errc::invalid_argument, .error_msg = "could not find ; in c instruction ,while parsing jmp string " };
	}
	else if (pos_of_semicolon == instruction.length() - 1) {
		return assembler_error{ .ec = std::errc::invalid_argument, .error_msg = "; is the last char in c instruction ,while parsing jmp string" };
	}
	return std::string{ instruction.data() + pos_of_semicolon + 1, instruction.length() - 1 };
}

static std::map<std::string, jump_mnemonic> jump_string_to_mnemonic_map{
	{"JEQ", jump_mnemonic::JEQ},
	{"JGE", jump_mnemonic::JGE},
	{"JGT", jump_mnemonic::JGT},
	{"JLE", jump_mnemonic::JLE},
	{"JLT", jump_mnemonic::JLT},
	{"JMP", jump_mnemonic::JMP},
	{"JMP", jump_mnemonic::JNE},
};
std::variant<jump_bincode,assembler_error> get_jmp_bincode(std::string jmp_string) {
	if (not jump_string_to_mnemonic_map.contains(jmp_string)) {
		return assembler_error{ .ec = std::errc::invalid_argument, .error_msg = std::format("couldnt map jump string {} to jump mnemonic while getting jump bincode", jmp_string) };
	}
	jump_mnemonic jmp_mnemonic = jump_string_to_mnemonic_map[jmp_string];
	return get_jmp_bincode(jmp_mnemonic);
}

static std::map<std::string, dest_mnemonic> dest_string_to_mnemonic_map{
	{"A", dest_mnemonic::A},
	{"AD", dest_mnemonic::AD},
	{"AM", dest_mnemonic::AM},
	{"AMD", dest_mnemonic::AMD},
	{"D", dest_mnemonic::D},
	{"M", dest_mnemonic::M},
	{"MD", dest_mnemonic::MD},
};

std::variant<dest_bincode, assembler_error> get_dest_bincode(std::string dest_string) {
	if (not dest_string_to_mnemonic_map.contains(dest_string)) {
		return assembler_error{ .ec = std::errc::invalid_argument, .error_msg = std::format("couldnt map dest string {} to dest mnemonic while getting dest bincode", dest_string) };
	}
	dest_mnemonic dest_mnemonic = dest_string_to_mnemonic_map[dest_string];
	return get_dest_bincode(dest_mnemonic);
}

bincode_result get_c_instruction_bincode(std::string instruction) {

	auto dest_part = get_dest_string(instruction);
	auto comp_part = get_comp_string(instruction);
	auto jmp_part = get_jmp_string(instruction);

	if (std::get_if<assembler_error>(&dest_part)) {
		auto dest_part_result_as_error = std::get<assembler_error>(dest_part);
		return assembler_error{ .ec = dest_part_result_as_error.ec, .error_msg = dest_part_result_as_error.error_msg };
	}
	if (std::get_if<assembler_error>(&comp_part)) {
		auto comp_part_result_as_error = std::get<assembler_error>(comp_part);
		return assembler_error{ .ec = comp_part_result_as_error.ec, .error_msg = comp_part_result_as_error.error_msg };
	}
	if (std::get_if<assembler_error>(&jmp_part)) {
		auto jmp_part_result_as_error = std::get<assembler_error>(jmp_part);
		return assembler_error{ .ec = jmp_part_result_as_error.ec, .error_msg = jmp_part_result_as_error.error_msg };
	}

	dest_part = std::get<std::string>(dest_part);
	comp_part = std::get<std::string>(comp_part);
	jmp_part = std::get<std::string>(jmp_part);


	return std::bitset<16>{};

}
std::bitset<16> instruction_string_to_bin(std::string instruction) {
	instruction.erase(std::remove(instruction.begin(), instruction.end(), ' '), instruction.end());
	if (instruction[0] == '@') {
		//return get_a_instruction_bincode(instruction).value();
	}
	else {
		//return get_c_instruction_bincode(instruction);
	}
	return 0;
}