#include <gtest/gtest.h>
#include "code_parser.h"
#include <vector>



TEST(ParserTest, RemoveCommentFromInstruction) {

	std::string instr = "// This file is part of www.nand2tetris.org";
	EXPECT_STREQ("", remove_comment_from_instruction(instr).c_str());

	instr = " blah // This file is part of www.nand2tetris.org";
	EXPECT_STREQ(" blah ", remove_comment_from_instruction(instr).c_str());

	instr = " blah ";
	EXPECT_STREQ(" blah ", remove_comment_from_instruction(instr).c_str());

	
}

TEST(ParserTest, AInstruction) {

	EXPECT_EQ(std::get<bincode>(parse_a_instruction_into_bincode("@5")), 0b0000000000000101);
}

TEST(ParserTest, CInstruction) {

	EXPECT_EQ(std::get<bincode>(parse_c_instruction_into_bincode("D-1")), 0b1110001110000000);
}

