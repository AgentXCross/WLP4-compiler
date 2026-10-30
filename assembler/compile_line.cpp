#include "assembler.hpp"


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

    if (opcode_key.length() == 4 &&
        opcode_key[0] == 'b' && opcode_key[1] == '.'
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
        instruction == "udiv"
    ) {
        std::int32_t xd{first}, xn{second}, xm{third};

        opcode_and_operands = opcodes.at(opcode_key);
        word = (opcode_and_operands) | (xm << 16) | (xn << 5) | (xd);
    } else if (instruction == "cmp") {
        std::int32_t xn{first}, xm{second};

        opcode_and_operands = opcodes.at(opcode_key);
        word = (opcode_and_operands) | (xm << 16) | (xn << 5);
    } else if (instruction == "br" || instruction == "blr") {
        std::int32_t xn{first};

        opcode_and_operands = opcodes.at(opcode_key);
        word = (opcode_and_operands) | (xn << 5);
    } else if (instruction == "ldur" || instruction == "stur") {
        std::int32_t xd{first}, xn{second}, immediate{third};

        if (immediate > 255 || immediate < -256) {
            format_error("ERROR: Invalid immediate value for ldur or stur");
            return false;
        }

        immediate = immediate & 0x1FF; // For negative immediates, 0x1FF is nine 1's
        
        opcode_and_operands = opcodes.at(opcode_key);
        word = (opcode_and_operands) | (immediate << 12) | (xn << 5) | (xd);
    } else if (instruction == "ldr") {
        std::int32_t xd{first}, immediate{second};
        if (immediate % 4 != 0) {
            format_error("ERROR: Immediate must be divisible by 4 for ldr");
            return false;
        }

        immediate /= 4;

        if (immediate > 262143 || immediate < -262144) {
            format_error("ERROR: Invalid immediate value for ldr");
            return false;
        }

        immediate = immediate & 0x7FFFF; // For negative immediates, 0x7FFFF is nineteen 1's

        opcode_and_operands = opcodes.at(opcode_key);
        word = (opcode_and_operands) | (immediate << 5) | (xd);
    } else if (instruction == "b") {
        std::int32_t immediate{first};
        if (immediate % 4 != 0) {
            format_error("ERROR: Immediate must be divisible by 4 for b");
            return false;
        }

        immediate /= 4;

        if (immediate > 33554431 || immediate < -33554432) {
            format_error("ERROR: Invalid immediate value for b");
            return false;
        }

        immediate = immediate & 0x3FFFFFF; // For negative immediates, 0x3FFFFFF is twenty-six 0's
        
        opcode_and_operands = opcodes.at(opcode_key);
        word = (opcode_and_operands) | (immediate);
    } else if (opcode_key == "b.cond") {
        std::string cond_code_str{instruction.substr(2, 2)};

        std::int32_t immediate{first};
        if (immediate % 4 != 0) {
            format_error("ERROR: Immediate must be divisible by 4 for b.cond");
            return false;
        }

        immediate /= 4;

        if (immediate > 262143 || immediate < -262144) {
            format_error("ERROR: Invalid immediate value for b.cond");
            return false;
        }

        immediate = immediate & 0x7FFFF; // For negative immediates, 0x7FFFF is nineteen 0's

        opcode_and_operands = opcodes.at(opcode_key);

        auto cond_it{condition_codes.find(cond_code_str)};

        if (cond_it == condition_codes.end()) {
            format_error("ERROR: Invalid condition code for b.cond");
            return false;
        }

        std::uint32_t cond_code{cond_it->second};

        word = (opcode_and_operands) | (immediate << 5) | (cond_code);
    } else {
        return false;
    }

    return true;
}
