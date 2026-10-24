#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

enum class TokenType {
    NONE,
    DOTID,
    LABEL,
    ID,
    HEXINT,
    REG,
    ZREG,
    SP,
    INT,
    COMMA,
    LBRACK,
    RBRACK,
    NEWLINE
};

struct Token {
    TokenType type;
    std::string lexeme;
};

void format_error(const std::string &message) {
    throw std::runtime_error(message);
}

void require(bool condition, const std::string &message) {
    if (!condition) format_error(message);
}


/**
 * @brief Assembles ARM64 assembly into little-endian machine code.
 * Supported ARM64 instructions
 *  add, sub, mul, umulh, smulh, sdiv, udiv
 *  cmp
 *  br, blr 
 *  ldur, stur 
 *  ldr
 *  b, b.cond -> cond = {eq, ne, hs, lo, hi, ls, ge, gt, le, lt}
 * Also supports the .8byte directive
 * 
 * Contains the instruction encoding tables, tokenizer DFA, and
 * functionality required for assembly. 
 * 
 * Source code, tokens, and generated machine code are 
 * processed locally and are not stored as instance members.
*/
class ARM64Assembler {
private:
    /** 
     * ASSIGNMENT 1
     * @brief compile_line returns true if the instruction is encoded properly into word, false otherwise
     * Only used for instructions, not for the .8byte directive.
     * The .8byte directive is handled separately in the assembler main function.
     * @param word Compiled instruction is put into word
     * @param instruction String of the instruction we're encoding (e.g. "add", "sub")
     * @param first The first argument of an instruction
     * @param second The second argument of an instruction (optional depending on instruction)
     * @param third The third argument of an instruction (optional depending on instruction)
     * @return true if compiled, false otherwise
    */
    bool compile_line(
        std::uint32_t &word,
        const std::string &instruction,
        std::int32_t first,
        std::int32_t second = 0,
        std::int32_t third = 0
    );

    /**
     * ASSEGINMENT 2
     * @brief Tokenizes one line of ARM64 assembly.
     * @param line The raw assembly line.
     * @return The tokens, including a trailing NEWLINE token for each line.
     */
    std::vector<Token> tokenize_line(const std::string &line);

    /**
     * ASSIGNMENT 2
     * @brief Builds the symbol table mapping each label to an address
     * Assumes the program starts at address 0x00
     * 
     * @param lines vector of all assembly lines
     * @return Symbol table as std::map<std::string, std::uint64_t> 
     */
    std::map<std::string, std::uint64_t> build_symbol_table(
        const std::vector<std::vector<Token>> &lines
    );

    /**
     * ASSIGNMENT 3
     * @brief resolve_and_validate_line resolves labels and validates a line of assembly
     * All HEXINT and Label usages are replaced with INT
     * Does not adjust immediate values for ldr, b, b.cond since compile_line handles it.
     * @param line A line is a vector of Tokens. Will be mutated.
     * @param labels Symbol table of the labels mapping label to where they are declared
     * @param current_address The current address of the program when processing this line.
     * @return void
     * 
     * b.cond is tokenized as 2 separate tokens: {ID, b}, {DOTID, .cond}
     * but is them merged into one token {ID, b.cond}
    */
    void resolve_and_validate_line(
        std::vector<Token> &line,
        const std::map<std::string, std::uint64_t> &labels,
        u_int64_t current_address
    );

public: 
    /**
     * @brief Assembles an entire ARM64 assembly program. 
              For each line, calls (in this order) tokenize_line, build_symbol_table, resolve_and_validate_line, compile_line
     * @param source The assembly source code.
     * @return The resulting little-endian machine code.
     */
    std::vector<std::uint8_t> assemble(const std::string &source);
};
