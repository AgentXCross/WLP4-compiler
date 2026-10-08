#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <unordered_set>
#include <map>
#include <stdexcept>

// For DFA
struct State {
    std::string name;
    bool accepting = false;
};

struct Transition {
    std::string from_state;
    std::vector<char> symbols;
    std::string to_state;
};

struct DFA {
    std::vector<char> alphabet;
    std::vector<State> states;
    std::vector<Transition> transitions;
};

enum TokenType {
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


void format_error(const std::string &message)
{
    throw std::runtime_error(message);
}

void require(
    bool condition,
    const std::string &message
) {
    if (!condition) {
        format_error(message);
    }
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
 * functionality required for assembly. Source code, tokens, and
 * generated machine code are processed locally and are not
 * stored as instance members.
*/
class ARM64Assembler {
private:
    DFA tokenizer_dfa;

    // condition stores the valid b.cond conditions and their binary encodings
    const std::map<std::string, int> condition_codes{
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

    /**
     * @brief Given a general-purpose register as a string (x30), returns the number portion as an integer
    */
    int read_reg(const std::string &reg) {
        int number{std::stoi(reg.substr(1))};

        require(
            number >= 0 && number <= 30,
            "ERROR: Invalid register"
        );

        return number;
    }

    // Constructs the DFA used for assembly tokenization.
    DFA create_tokenizer_dfa();

    // Returns the next state for a given input character.
    State delta(const State &current, char symbol) const;

    // Checks whether a state is accepting.
    bool is_state_accepting(const std::string &state_name) const;

    // Converts an accepting state into a TokenType.
    TokenType return_kind(const std::string &state_name) const;

public: 
    // Constructor method
    ARM64Assembler() : tokenizer_dfa{create_tokenizer_dfa()} {}

    /** 
     * @brief compile_line returns true if the instruction is encoded properly into word, false otherwise
     * Only used for instructions, not for the .8byte directive.
     * The .8byte directive is handled separately in the assembler main function.
     * @param word Compiled instruction is put into word
     * @param instruction String of the instruction we're encoding (e.g. "add", "sub")
     * @param one The first argument of an instruction
     * @param two The second argument of an instruction (optional depending on instruction)
     * @param three The third argument of an instruction (optional depending on instruction)
     * @return true if compiled, false otherwise
    */
    bool compile_line(
        u_int32_t &word,
        const std::string &instruction,
        std::size_t one,
        std::size_t two = 0,
        std::size_t three = 0
    );

    /**
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

    /**
     * @brief Tokenizes one line of ARM64 assembly.
     * @param line The raw assembly line.
     * @return The tokens, including a trailing NEWLINE token for each line.
     */
    std::vector<Token> tokenize_line(const std::string &line);

    /**
     * @brief Assembles an entire ARM64 assembly program. 
              Calls (in this order) tokenize_line, resolve_and_validate_line, compile_line
     * @param source The assembly source code.
     * @return The resulting little-endian machine code.
     */
    std::vector<std::uint8_t> assemble(const std::string &source);
};
