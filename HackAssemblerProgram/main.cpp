#include "code_parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

int main() {
	std::filesystem::path input_filepath{ "C:/Program Files/nand2tetris/projects/06/rect/RectL.asm"};
	std::filesystem::path output_filepath{ "hack_res.hack"};

	std::ifstream input_file{ input_filepath};
	if (not input_file) {
		std::cerr << "couldnt open input file";
		return -1;
	}
	std::ofstream output_file{ output_filepath };

	if (not output_file.good()) {
		std::cerr << "couldnt open ouptut file";
		return -1;
	}

	std::stringstream input_file_buf{};
	input_file_buf << input_file.rdbuf();
	std::string input = input_file_buf.str();

	//std::cout << input;

	auto assembler_output = string_program_to_bincode(input);

	bool any_errors_occurred = false;

	for (auto bin : assembler_output) {
		if (std::get_if<assembler_error>(&bin)) {
			std::cout << std::get<assembler_error>(bin).error_msg << std::endl;
			any_errors_occurred = true;
		}
	}
	if (any_errors_occurred) {
		return -1;
	}

	for (auto bin : assembler_output) {
		output_file << std::get<bincode>(bin) << "\n";
	}


	return 0;
}