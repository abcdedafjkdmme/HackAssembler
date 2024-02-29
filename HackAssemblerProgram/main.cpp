#include "code_parser.h"
#include <iostream>
#include <fstream>
#include <sstream>

int main() {
	std::string input_filepath{};
	std::string output_filepath{};

	/*std::cout << "print input file path \n";
	std::cin >> input_filepath;

	std::cout << "print output file path \n";
	std::cin >> output_filepath;*/

	std::ifstream input_file{ "C:/Program Files/nand2tetris/projects/06/add/add.asm" };

	std::stringstream input_file_buf{};
	input_file_buf << input_file.rdbuf();
	std::string input = input_file_buf.str();

	auto output = string_program_to_bincode(input);


	for (auto bin : output) {
		std::cout << std::get<bincode>(bin) << std::endl;
	}
	return 0;
}