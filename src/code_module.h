#pragma once
#include <cstdint>
#include <bitset>

enum class dest_mnemonic { none, M, D, MD, A, AM, AD, AMD };
enum class jump_mnemonic { none, JGT, JEQ, JGE, JLT, JNE, JLE, JMP };
enum class comp_mnemonic { none, JGT, JEQ, JGE, JLT, JNE, JLE, JMP };
enum class ALU_input { minus_one, plus_one, zero, A, D, M };
enum class ALU_operation { none, bit_not, add, subtract, bit_and, bit_or };

struct dest_bincode {
  bool d1;
  bool d2;
  bool d3;
};
struct jump_bincode {
  bool j1;
  bool j2;
  bool j3;
};
struct comp_bincode {
  bool a;
  bool c1;
  bool c2;
  bool c3;
  bool c4;
  bool c5;
  bool c6;
};

std::bitset<16> get_a_instruction_bincode(uint16_t value) {
    std::bitset<16> instr = value;
    instr.reset(15);
    return instr;
}
dest_bincode get_dest_bincode(dest_mnemonic mnemonic) {

  switch (mnemonic) {
  case dest_mnemonic::none:
    return dest_bincode{0, 0, 0};
  case dest_mnemonic::M:
    return dest_bincode{0, 0, 1};
  case dest_mnemonic::D:
    return dest_bincode{0, 1, 0};
  case dest_mnemonic::MD:
    return dest_bincode{0, 1, 1};
  case dest_mnemonic::A:
    return dest_bincode{1, 0, 0};
  case dest_mnemonic::AM:
    return dest_bincode{1, 0, 1};
  case dest_mnemonic::AD:
    return dest_bincode{1, 1, 0};
  case dest_mnemonic::AMD:
    return dest_bincode{1, 1, 1};
  }
}
jump_bincode get_jmp_bincode(jump_mnemonic mnemonic) {

  switch (mnemonic) {
  case jump_mnemonic::none:
    return jump_bincode{0, 0, 0};
  case jump_mnemonic::JGT:
    return jump_bincode{0, 0, 1};
  case jump_mnemonic::JEQ:
    return jump_bincode{0, 1, 0};
  case jump_mnemonic::JGE:
    return jump_bincode{0, 1, 1};
  case jump_mnemonic::JLT:
    return jump_bincode{1, 0, 0};
  case jump_mnemonic::JNE:
    return jump_bincode{1, 0, 1};
  case jump_mnemonic::JLE:
    return jump_bincode{1, 1, 0};
  case jump_mnemonic::JMP:
    return jump_bincode{1, 1, 1};
  default:
      assert(false);
      return jump_bincode{};
  }
}

comp_bincode get_comp_bincode(ALU_input input1, ALU_input input2,
                              ALU_operation operation) {
  switch (operation) {
  case ALU_operation::none: {
    if (input1 == ALU_input::zero and input2 == ALU_input::zero) {
      return comp_bincode{1, 0, 1, 0, 1, 0};
    }
  }
  }
}
