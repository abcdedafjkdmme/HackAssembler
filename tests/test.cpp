#include <gtest/gtest.h>
#include "code_parser.h"

// Demonstrate some basic assertions.
TEST(ParserTest, GetCInstructionDestString) {

	std::string dest = get_dest_string("AM=3+A;JMP").value();
	EXPECT_STREQ(dest.c_str(), "AM");

}

TEST(ParserTest, GetCInstructionCompString) {

	EXPECT_STREQ(get_comp_string("AM=3+A;JMP").c_str(), "3+A");

	EXPECT_STREQ(get_comp_string("AM=3+A").c_str(), "3+A");

	EXPECT_STREQ(get_comp_string("AM=3+A;").c_str(), "3+A");

}

TEST(ParserTest, GetAInstructionBincode) {

	EXPECT_EQ(get_a_instruction_bincode("@432").value(), 432);
	EXPECT_EQ(get_a_instruction_bincode("@1").value(), 1);

}