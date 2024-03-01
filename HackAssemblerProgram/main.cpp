#include "code_parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

int main() {
	std::filesystem::path input_filepath{ "C:/Program Files/nand2tetris/projects/06/rect/RectL.asm"};
	std::filesystem::path output_filepath{ "D:/VsProjs/nand2tetris/rect.hack"};
	std::filesystem::path output_bin_filepath{ "D:/VsProjs/nand2tetris/rect.bin" };

	std::ifstream input_file{ input_filepath};
	if (not input_file) {
		std::cerr << "couldnt open input file";
		return -1;
	}
	std::ofstream output_file{ output_filepath };
	if (not output_file) {
		std::cerr << "couldnt open ouptut file";
		return -1;
	}

	std::ofstream output_file_bin{ output_bin_filepath, std::iostream::binary };
	if (not output_file_bin) {
		std::cerr << "couldnt open ouptut bin file";
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

	std::string output_file_bin_buffer{};

	for (auto bin : assembler_output) {
		const char output_bin_1 = (char)(std::get<bincode>(bin).to_ulong() >> 8);
		const char output_bin_2 = (char)(std::get<bincode>(bin).to_ulong() & 0b0000000011111111);
		output_file_bin_buffer.push_back(output_bin_1);
		output_file_bin_buffer.push_back(output_bin_2);

		output_file << std::get<bincode>(bin) << "\n";

		std::cout << std::hex << std::get<bincode>(bin).to_ulong() << "\n";
		std::cout << std::bitset<8>(std::get<bincode>(bin).to_ulong() >> 8);
		std::cout << std::bitset<8>(std::get<bincode>(bin).to_ulong() & 0b0000000011111111);

		std::cout << "\n\n";

	}
	std::cout << "\n\n";


	output_file_bin.write(output_file_bin_buffer.data(), output_file_bin_buffer.size());

	return 0;
}