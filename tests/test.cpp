#include <gtest/gtest.h>
#include "code_parser.h"
#include <vector>


TEST(ParserTest, AInstruction) {

  EXPECT_EQ(std::get<bincode>(parse_a_instruction_into_bincode("@5")), 0b0000000000000101);
}

TEST(ParserTest, CInstruction) {

	EXPECT_EQ(std::get<bincode>(parse_c_instruction_into_bincode("D-1")), 0b1110001110000000);
}

