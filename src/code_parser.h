#pragma once
#include "assembler_defines.h"
#include "code_module.h"
#include "utiils.h"
#include <algorithm>
#include <bitset>
#include <cassert>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <format>
#include <map>
#include <optional>
#include <string>
#include <variant>

bool does_instruction_have_dest_part(std::string instruction) {
	return instruction.find('=') != std::string::npos;
}

bool does_instruction_have_comp_part(std::string instruction) { return true; }

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

	auto parse_into_int_result = std::from_chars(
		instruction.data(), instruction.data() + instruction.size(), value, base);

	if (value > pow(15, 2)) {
		return assembler_error{ .ec = std::errc::value_too_large,
							   .error_msg =
								   "value in a instruction is larger than 15 bits" };
	}
	if ((int)parse_into_int_result.ec == 0) {
		return bincode{ get_a_instruction_bincode(value) };
	}
	else {
		return assembler_error{ .ec = std::errc::invalid_argument,
							   .error_msg =
								   "couldnt parse a instruction into valid int" };
	}
}

parser_string_result get_dest_string(std::string instruction) {
	size_t pos_of_equ = instruction.find('=');
	if (pos_of_equ == std::string::npos) {
		return assembler_error{ .ec = std::errc::invalid_argument,
							   .error_msg = " '=' not found in c instruction" };
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
		return assembler_error{ .ec = std::errc::invalid_argument,
							   .error_msg =
								   "'=' not found in c instruction and the first "
								   "char isnt 0 ,while parsing comp string" };
	}

	if ((pos_of_semicolon != std::string::npos) and
		(pos_of_semicolon < pos_of_equ)) {
		return assembler_error{ .ec = std::errc::invalid_argument,
							   .error_msg =
								   " ; is placed before = in c instruction ,while "
								   "parsing comp string" };
	}

	std::string comp_part{};

	if (pos_of_semicolon != std::string::npos) {
		comp_part = std::string{ instruction.data() + pos_of_equ + 1,
								pos_of_semicolon - pos_of_equ - 1 };
	}
	else {
		comp_part = std::string{ instruction.data() + pos_of_equ + 1,
								instruction.length() - pos_of_equ - 1 };
	}

	return comp_part;
}

parser_string_result get_jmp_string(std::string instruction) {
	size_t pos_of_semicolon = instruction.find(';');
	if (pos_of_semicolon == std::string::npos) {
		return assembler_error{
			.ec = std::errc::invalid_argument,
			.error_msg =
				"could not find ; in c instruction ,while parsing jmp string " };
	}
	else if (pos_of_semicolon == instruction.length() - 1) {
		return assembler_error{
			.ec = std::errc::invalid_argument,
			.error_msg =
				"; is the last char in c instruction ,while parsing jmp string" };
	}
	return std::string{ instruction.data() + pos_of_semicolon + 1,
					   instruction.length() - 1 };
}

static std::map<std::string, jump_mnemonic> jump_string_to_mnemonic_map{
	{"JEQ", jump_mnemonic::JEQ}, {"JGE", jump_mnemonic::JGE},
	{"JGT", jump_mnemonic::JGT}, {"JLE", jump_mnemonic::JLE},
	{"JLT", jump_mnemonic::JLT}, {"JMP", jump_mnemonic::JMP},
	{"JMP", jump_mnemonic::JNE},
};
std::variant<jump_bincode, assembler_error>
parse_string_to_jmp_bincode(std::string jmp_string) {
	if (not jump_string_to_mnemonic_map.contains(jmp_string)) {
		return assembler_error{
			.ec = std::errc::invalid_argument,
			.error_msg = std::format("couldnt map jump string {} to jump mnemonic "
									 "while getting jump bincode",
									 jmp_string) };
	}
	jump_mnemonic jmp_mnemonic = jump_string_to_mnemonic_map[jmp_string];
	return get_jmp_bincode(jmp_mnemonic);
}

static std::map<std::string, dest_mnemonic> dest_string_to_mnemonic_map{
	{"A", dest_mnemonic::A},   {"AD", dest_mnemonic::AD},
	{"AM", dest_mnemonic::AM}, {"AMD", dest_mnemonic::AMD},
	{"D", dest_mnemonic::D},   {"M", dest_mnemonic::M},
	{"MD", dest_mnemonic::MD},
};

std::variant<dest_bincode, assembler_error>
parse_string_into_dest_bincode(std::string dest_string) {
	if (not dest_string_to_mnemonic_map.contains(dest_string)) {
		return assembler_error{
			.ec = std::errc::invalid_argument,
			.error_msg = std::format("couldnt map dest string {} to dest mnemonic "
									 "while getting dest bincode",
									 dest_string) };
	}
	dest_mnemonic dest_mnemonic = dest_string_to_mnemonic_map[dest_string];
	return get_dest_bincode(dest_mnemonic);
}

static std::map<char, ALU_input> string_to_alu_input_map{
	{'A', ALU_input::A},   {'D', ALU_input::D},    {'M', ALU_input::M},
	{'1', ALU_input::one}, {'0', ALU_input::zero},
};
static std::map<char, ALU_operation> string_to_alu_operation_map{
	{'+', ALU_operation::add},      {'&', ALU_operation::bit_and},
	{'!', ALU_operation::bit_not},  {'|', ALU_operation::bit_or},
	{'-', ALU_operation::subtract},
};

static std::map<std::string, std::bitset<7>> string_to_comp_bincode_map_reverse_order{
	{"0",  0b0101010},
	{"1",  0b0111111},
	{"-1", 0b0111010},
	{"D" , 0b0001100},
	{"A" , 0b0110000},
	{"M" , 0b1110000},
	{"!D" ,0b0001101},
	{"!A", 0b0110001},
	{"!M", 0b1110001},
	{"-D", 0b0001111},
	{"-A", 0b0110011},
	{"-M", 0b1110011},
	{"D+1",0b0111111},
	{"A+1",0b0110111},
	{"M+1",0b1110111},
	{"D-1",0b0001110},
	{"A-1",0b0110010},
	{"M-1",0b1110010},
};

std::variant<comp_bincode, assembler_error>
parse_string_into_comp_bincode(std::string comp_string) {

	if (comp_string.size() == 1) {

		if (not string_to_alu_input_map.contains(comp_string[0])) {
			return assembler_error{
				.ec = std::errc::invalid_argument,
				.error_msg = "no operation instruction's first input isnt valid" };
		}

		ALU_input input = string_to_alu_input_map[comp_string[0]];
		return get_comp_bincode(input);
	}

	else if (comp_string.size() == 2) {

		char alu_operation_char = comp_string[0];
		char alu_input_char = comp_string[1];

		if (not string_to_alu_operation_map.contains(alu_operation_char)) {
			return assembler_error{
				.ec = std::errc::invalid_argument,
				.error_msg = "1 input operation's 1st char isnt valid operation" };
		}
		if (not string_to_alu_input_map.contains(comp_string[1])) {
			return assembler_error{
				.ec = std::errc::invalid_argument,
				.error_msg = "1 input operation's 2nd char isnt valid input" };
		}

		ALU_operation alu_operation =
			string_to_alu_operation_map[alu_operation_char];
		ALU_input alu_input = string_to_alu_input_map[alu_input_char];

		return get_comp_bincode(alu_input, alu_operation);
	}

	else if (comp_string.size() == 3) {

		char alu_input1_char = comp_string[0];
		char alu_operation_char = comp_string[1];
		char alu_input2_char = comp_string[2];

		if (not string_to_alu_input_map.contains(alu_input1_char)) {
			return assembler_error{
				.ec = std::errc::invalid_argument,
				.error_msg = "2 input operation's 1st char isnt valid input" };
		}
		if (not string_to_alu_input_map.contains(alu_input2_char)) {
			return assembler_error{
				.ec = std::errc::invalid_argument,
				.error_msg = "2 input operation's 2nd char isnt valid input" };
		}

		ALU_input alu_input1{ string_to_alu_input_map[alu_input1_char] };
		ALU_input alu_input2{ string_to_alu_input_map[alu_input2_char] };
		ALU_operation alu_operation =
			string_to_alu_operation_map[alu_operation_char];

		return get_comp_bincode(alu_input1, alu_input2, alu_operation);

	}
	else {
		return assembler_error{
			.ec = std::errc::invalid_argument,
			.error_msg = "error couldnt parse compute part of c instruction" };
	}
}

bincode_result get_c_instruction_bincode(std::string instruction) {

	auto dest_part = get_dest_string(instruction);
	auto comp_part = get_comp_string(instruction);
	auto jmp_part = get_jmp_string(instruction);

	if (std::get_if<assembler_error>(&dest_part)) {
		auto dest_part_result_as_error = std::get<assembler_error>(dest_part);
		return assembler_error{ .ec = dest_part_result_as_error.ec,
							   .error_msg = dest_part_result_as_error.error_msg };
	}
	if (std::get_if<assembler_error>(&comp_part)) {
		auto comp_part_result_as_error = std::get<assembler_error>(comp_part);
		return assembler_error{ .ec = comp_part_result_as_error.ec,
							   .error_msg = comp_part_result_as_error.error_msg };
	}
	if (std::get_if<assembler_error>(&jmp_part)) {
		auto jmp_part_result_as_error = std::get<assembler_error>(jmp_part);
		return assembler_error{ .ec = jmp_part_result_as_error.ec,
							   .error_msg = jmp_part_result_as_error.error_msg };
	}

	bincode dest_part_bincode{};
	bincode comp_part_bincode{};
	bincode jmp_part_bincode{};

	if (does_instruction_have_dest_part(instruction)) {
		// dest_part_bincode = parse_string_into_dest_bincode()
	}

	return std::bitset<16>{};
}
std::bitset<16> instruction_string_to_bin(std::string instruction) {
	instruction.erase(std::remove(instruction.begin(), instruction.end(), ' '),
		instruction.end());
	if (instruction[0] == '@') {
		// return get_a_instruction_bincode(instruction).value();
	}
	else {
		// return get_c_instruction_bincode(instruction);
	}
	return 0;
}
