#include <gtest/gtest.h>
#include "code_parser.h"
#include <vector>

// Demonstrate some basic assertions.
TEST(ParserTest, GetCInstructionDestString) {

	std::string dest = std::get<std::string>(get_dest_string("AM=3+A;JMP"));
	EXPECT_STREQ(dest.c_str(), "AM");

}

TEST(ParserTest, GetCInstructionCompString) {

	EXPECT_STREQ(std::get<std::string>(get_comp_string("AM=3+A;JMP")).c_str(), "3+A");

	EXPECT_STREQ(std::get<std::string>(get_comp_string("AM=3+AD")).c_str(), "3+AD");

	EXPECT_STREQ(std::get<std::string>(get_comp_string("AM=3+A;")).c_str(), "3+A");

}

TEST(ParserTest, GetCInstructionJMPString) {

	EXPECT_STREQ(std::get<std::string>(get_jmp_string("AM=3+A;JMP")).c_str(), "JMP");
	EXPECT_EQ(std::get<assembler_error>(get_jmp_string("AM=3+A")).ec, std::errc::invalid_argument);

}


TEST(ParserTest, GetCInstructionCompBincode) {


	comp_bincode bin = std::get<comp_bincode>(parse_string_into_comp_bincode("0"));
	comp_bincode compare = comp_bincode{ 0,1,0,1,0,1,0 };
	EXPECT_EQ(bin, compare);

	bin = std::get<comp_bincode>(parse_string_into_comp_bincode("1"));
	compare = comp_bincode{ 0,1,1,1,1,1,1 };
	EXPECT_EQ(bin, compare);

	bin = std::get<comp_bincode>(parse_string_into_comp_bincode("-1"));
	compare = comp_bincode{ 0,1,1,1,0,1,0 };
	EXPECT_EQ(bin, compare);


	bin = std::get<comp_bincode>(parse_string_into_comp_bincode("D"));
	compare = comp_bincode{ 0, 0,0,1,1,0,0 };
	EXPECT_EQ(bin, compare);

	bin = std::get<comp_bincode>(parse_string_into_comp_bincode("A"));
	compare = comp_bincode{ 0, 1, 1, 0,0,0,0 };
	EXPECT_EQ(bin, compare);

	bin = std::get<comp_bincode>(parse_string_into_comp_bincode("M"));
	compare = comp_bincode{ 1, 1, 1, 0,0,0,0 };
	EXPECT_EQ(bin, compare);


}

TEST(ParserTest, GetAInstructionBincode) {

	EXPECT_EQ(std::get<bincode>(get_a_instruction_bincode("@0b101")), 0b1010);
	EXPECT_EQ(std::get<bincode>(get_a_instruction_bincode("@1")), 0b10);

}