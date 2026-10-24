#include "assembler.hpp"

#include <string>


static const std::map<std::string, std::uint32_t> condition_codes{
        {"eq", 0b00000},
        {"ne", 0b00001},
        {"hs", 0b00010}, 
        {"lo", 0b00011}, 
        {"hi", 0b01000}, 
        {"ls", 0b01001}, 
        {"ge", 0b01010}, 
        {"gt", 0b01100}, 
        {"le", 0b01101}, 
        {"lt", 0b01011}
};

static const std::map<std::string, std::uint32_t> opcodes{
    {"add",     0b10001011001000000110000000000000},
    {"sub",     0b11001011001000000110000000000000},
    {"mul",     0b10011011000000000111110000000000},
    {"smulh",   0b10011011010000000111110000000000},
    {"umulh",   0b10011011110000000111110000000000},
    {"sdiv",    0b10011010110000000000110000000000},
    {"udiv",    0b10011010110000000000100000000000},
    {"cmp",     0b11101011001000000110000000011111},
    {"br",      0b11010110000111110000000000000000},
    {"blr",     0b11010110001111110000000000000000},
    {"ldur",    0b11111000010000000000000000000000},
    {"stur",    0b11111000000000000000000000000000}, 
    {"ldr",     0b01011000000000000000000000000000},
    {"b",       0b00010100000000000000000000000000},
    {"b.cond",  0b01010100000000000000000000000000}
};



bool ARM64Assembler::compile_line(
    std::uint32_t &word,
    const std::string &instruction,
    std::int32_t first,
    std::int32_t second,
    std::int32_t third 
) {
    word = 0;
    std::uint32_t opcode_and_operands{0};

    std::string opcode_key{instruction};

    if (opcode_key.length() > 3 &&
        opcode_key[0] == 'b' && opcode_key[1] == 'b.'
    ) {
        opcode_key = "b.cond";
    }

    auto it{opcodes.find(opcode_key)};

    if (it == opcodes.end()) {
        format_error("ERROR: Invalid Instruction");
        return false;
    }

    if (instruction == "add" || instruction == "sub" || instruction == "mul" ||
        instruction == "smulh" || instruction == "umulh" || instruction == "sdiv" ||
        instruction == "udiv" || instruction == "sdiv"
    ) {
        std::int32_t xd{first}, xn{second}, xm{third};

        opcode_and_operands = opcodes.at(opcode_key);
        word = (opcode_and_operands) | (xm << 16) | (xn << 5) | (xd);
    }

}