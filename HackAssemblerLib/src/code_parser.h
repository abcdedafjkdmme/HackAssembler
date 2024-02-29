#pragma once
#include "assembler_defines.h"
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

bool is_a_instruction(std::string instruction) {
	return instruction[0] == '@';
}

bool does_instruction_have_dest_part(std::string instruction) {
	return instruction.find('=') != std::string::npos;
}

bool does_instruction_have_comp_part(std::string instruction) { return true; }

bool does_instruction_have_jmp_part(std::string instruction) {
	return instruction.find(';') != std::string::npos;
}

bincode_result parse_a_instruction_into_bincode(std::string instruction) {

	instruction.erase(0, 1);

	uint16_t value{};
	auto parse_into_int_result = std::from_chars(
		instruction.data(), instruction.data() + instruction.size(), value);

	if (value > pow(15, 2)) {
		return assembler_error{ .ec = std::errc::value_too_large,
							   .error_msg =
								   "value in a instruction is larger than 15 bits" };
	}
	if ((int)parse_into_int_result.ec == 0) {
		bincode ret = value;
		ret.reset(15);
		return ret;
	}
	else {
		return assembler_error{ .ec = std::errc::invalid_argument,
							   .error_msg =
								   "couldnt parse a instruction into valid int" };
	}
}

parser_string_result get_dest_string(std::string instruction) {
	/*
	size_t pos_of_equ = instruction.find('=');
	if (pos_of_equ == std::string::npos) {
		return assembler_error{ .ec = std::errc::invalid_argument,
							   .error_msg = " '=' not found in c instruction" };
	}

	return std::string{ instruction.data(), pos_of_equ };
	*/

	auto split = assembler_utils::split_string(instruction, "=");
	if (split.size() > 2) {
		return assembler_error{ std::errc::invalid_argument, "incorrect amount of = in c instruction" };
	}
	if (split.size() == 1) {
		return "";
	}
	else {
		return split[0];
	}
}

parser_string_result get_jmp_string(std::string instruction) {
	/*
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
	*/
	auto split = assembler_utils::split_string(instruction, ";");
	if (split.size() > 2) {
		return assembler_error{ std::errc::invalid_argument, "incorrect amount of ; in c instruction" };
	}
	else if (split.size() == 1) {
		return "";
	}
	else if(split.size() == 2) {
		return split[1];
	}
	assert(false);

}

parser_string_result get_comp_string(std::string instruction) {
	/*
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
	*/

	auto split = assembler_utils::split_string(instruction, "=");
	if (split.size() > 2) {
		return assembler_error{ std::errc::invalid_argument, "incorrect amount of = in c instruction" };
	}
	if (split.size() == 1) {
		auto split_2 = assembler_utils::split_string(split[0], ";");
		return split_2[0];
	}
	else {
		auto split_2 = assembler_utils::split_string(split[1], ";");
		return split_2[0];
	}
}


static std::map<std::string, dest_bincode> string_to_dest_bincode_map{
	{"M",  0b001},
	{"D",  0b010},
	{"MD", 0b011},
	{"A",  0b100},
	{"AM", 0b101},
	{"AD", 0b110},
	{"AMD",0b111},
};

static std::map<std::string, jmp_bincode> string_to_jmp_bincode_map{
	{"JGT",0b001},
	{"JEQ",0b010},
	{"JGE",0b011},
	{"JLT",0b100},
	{"JNE",0b101},
	{"JLE",0b110},
	{"JMP",0b111},
};

static std::map<std::string, comp_bincode> string_to_comp_bincode_map{
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
	{"D+A",0b0000010},
	{"D+M",0b1000010},
	{"D-A",0b0010011},
	{"D-M",0b1010011},
	{"A-D",0b0000111},
	{"M-D",0b1000111},
	{"D&A",0b0000000},
	{"D&A",0b1000000},
	{"D|A",0b0010101},
	{"D|M",0b1010101},
};

std::variant<dest_bincode, assembler_error> parse_string_into_dest_bincode(std::string dest_string) {
	if (not string_to_dest_bincode_map.contains(dest_string)) {
		return assembler_error{ std::errc::invalid_argument, "couldnt parse dest string" };
	}
	return string_to_dest_bincode_map[dest_string];
}

std::variant<comp_bincode, assembler_error> parse_string_into_comp_bincode(std::string comp_string) {
	if (not string_to_comp_bincode_map.contains(comp_string)) {
		return assembler_error{ std::errc::invalid_argument, "couldnt parse comp string" };
	}
	return string_to_comp_bincode_map[comp_string];
}

std::variant<jmp_bincode, assembler_error> parse_string_into_jmp_bincode(std::string jmp_string) {
	if (not string_to_jmp_bincode_map.contains(jmp_string)) {
		return assembler_error{ std::errc::invalid_argument, "couldnt parse jmp string" };
	}
	return string_to_jmp_bincode_map[jmp_string];
}

bincode_result parse_c_instruction_into_bincode(std::string instruction) {

	auto dest_part = get_dest_string(instruction);
	auto comp_part = get_comp_string(instruction);
	auto jmp_part = get_jmp_string(instruction);

	if (std::get_if<assembler_error>(&dest_part)) {
		return std::get<assembler_error>(dest_part);

	}
	if (std::get_if<assembler_error>(&comp_part)) {
		return std::get<assembler_error>(comp_part);

	}
	if (std::get_if<assembler_error>(&jmp_part)) {
		return std::get<assembler_error>(jmp_part);
	}

	std::variant<dest_bincode, assembler_error> dest_part_bincode = dest_bincode{};
	std::variant<comp_bincode, assembler_error> comp_part_bincode = comp_bincode{};
	std::variant<jmp_bincode, assembler_error> jmp_part_bincode = jmp_bincode{};

	if (does_instruction_have_dest_part(instruction)) {
		dest_part_bincode = parse_string_into_dest_bincode(std::get<std::string>(dest_part));
	}
	if (does_instruction_have_comp_part(instruction)) {
		comp_part_bincode = parse_string_into_comp_bincode(std::get < std::string>(comp_part));
	}
	if (does_instruction_have_jmp_part(instruction)) {
		jmp_part_bincode = parse_string_into_jmp_bincode(std::get < std::string>(jmp_part));
	}

	if (std::get_if<assembler_error>(&dest_part_bincode)) {
		return std::get<assembler_error>(dest_part_bincode);
	}
	if (std::get_if<assembler_error>(&comp_part_bincode)) {
		return std::get<assembler_error>(comp_part_bincode);
	}
	if (std::get_if<assembler_error>(&jmp_part_bincode)) {
		return std::get<assembler_error>(jmp_part_bincode);
	}

	bincode ret{};

	ret[0] = std::get<jmp_bincode>(jmp_part_bincode)[0];
	ret[1] = std::get<jmp_bincode>(jmp_part_bincode)[1];
	ret[2] = std::get<jmp_bincode>(jmp_part_bincode)[2];

	ret[3] = std::get<dest_bincode>(dest_part_bincode)[0];
	ret[4] = std::get<dest_bincode>(dest_part_bincode)[1];
	ret[5] = std::get<dest_bincode>(dest_part_bincode)[2];

	ret[6]  = std::get<comp_bincode>(comp_part_bincode)[0];
	ret[7]  = std::get<comp_bincode>(comp_part_bincode)[1];
	ret[8]  = std::get<comp_bincode>(comp_part_bincode)[2];
	ret[9]  = std::get<comp_bincode>(comp_part_bincode)[3];
	ret[10] = std::get<comp_bincode>(comp_part_bincode)[4];
	ret[11] = std::get<comp_bincode>(comp_part_bincode)[5];
	ret[12] = std::get<comp_bincode>(comp_part_bincode)[6];

	ret[13] = 1;
	ret[14] = 1;
	ret[15] = 1;

	return ret;
}

bincode_result instruction_string_to_bincode(std::string instruction) {
	instruction.erase(std::remove(instruction.begin(), instruction.end(), ' '),
		instruction.end());

	if (is_a_instruction(instruction)) {
		 return parse_a_instruction_into_bincode(instruction);
	}
	else {
		return parse_c_instruction_into_bincode(instruction);
	}
	return 0;
}
