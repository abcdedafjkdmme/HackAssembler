#pragma once
#include <cassert>
#include <string>
#include <bitset>
#include <algorithm>
#include <charconv>
#include <cstdint>
#include "code_module.h"
#include <optional>

std::optional<std::bitset<16>> get_a_instruction_bincode(std::string instruction) {
	uint16_t value{};
	auto result = std::from_chars(instruction.data() + 1, instruction.data() + instruction.size(), value);
	if ((int)result.ec == 0) {
		return get_a_instruction_bincode(value);
	}
	else {
		return {};
	}
}

std::optional<std::string> get_dest_string(std::string instruction) {
	size_t pos_of_equ = instruction.find('=');
	if (pos_of_equ != std::string::npos) {
		return std::string{ instruction.data(), pos_of_equ };
	}
	else {
		return {};
	}

}

std::string get_comp_string(std::string instruction) {
	size_t pos_of_equ = instruction.find('=');
	size_t pos_of_semicolon = instruction.find(';');
	std::string comp_part{  };

	if (pos_of_semicolon != std::string::npos and pos_of_equ != std::string::npos) {
		assert(pos_of_semicolon > pos_of_equ);
		comp_part = std::string{ instruction.data() + pos_of_equ + 1,  pos_of_semicolon - pos_of_equ - 1};
	}
	else {
		assert(pos_of_equ < instruction.length());
		comp_part = std::string{ instruction.data() + pos_of_equ + 1, instruction.length() - pos_of_equ - 1};
	}

	return comp_part;
}

std::bitset<16> get_c_instruction_bincode(std::string instruction) {
	
	std::string dest_part = get_dest_string(instruction).value();
	std::string comp_part = get_comp_string(instruction);
	
	return std::bitset<16>{};

}
std::bitset<16> instruction_string_to_bin(std::string instruction) {
	instruction.erase(std::remove(instruction.begin(), instruction.end(), ' '), instruction.end());
	if (instruction[0] == '@') {
		return get_a_instruction_bincode(instruction).value();
	}
	else {
		return get_c_instruction_bincode(instruction);
	}
}