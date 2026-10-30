#include "assembler.hpp"
#include "arm64_dfa.hpp"


static TokenType return_kind(std::string accepting_state) {
    if (accepting_state == "dot_alpha") {
        return TokenType::DOTID;
    } else if (accepting_state == "x" || accepting_state == "xz" || accepting_state == "s" ||
                accepting_state == "first_letter" || accepting_state == "first_letter_then_alpha"
    ) {
        return TokenType::ID;
    } else if (accepting_state == "xzr") {
        return TokenType::ZREG;
    } else if (accepting_state == "x_one_digit" || accepting_state == "x_two_digits") {
        return TokenType::REG;
    } else if (accepting_state == "sp") {
        return TokenType::SP;
    } else if (accepting_state == "0" || accepting_state == "first1-9" || 
        accepting_state == "first1-9_then_digits" || 
        accepting_state == "negative_sign_then_digits"
    ) {
        return TokenType::INT;
    } else if (accepting_state == "0x_hex") {
        return TokenType::HEXINT;
    } else if (accepting_state == "comma") {
        return TokenType::COMMA;
    } else if (accepting_state == "left_bracket") {
        return TokenType::LBRACK;
    } else if (accepting_state == "right_bracket") {
        return TokenType::RBRACK;
    } else if (accepting_state == "colon") {
        return TokenType::LABEL;
    } else {
        format_error("ERROR: Invalid accepting state for a token");
        return TokenType::NONE;
    }
}

static bool char_in_vector(const std::vector<char> &vec, const char c) {
    for (const char &i : vec) {
        if (i == c) {
            return true;
        }
    }

    return false;
}

static bool is_state_accepting(const std::vector<State> &states, const std::string state_name) {
    for (const State &s : states) {
        if (state_name == s.state_name) {
            return s.accepting;
        }
    }

    return false;
}

bool is_char(const std::string s) {
    return s.length() == 1;
}

bool is_range(const std::string s) {
    return s.length() == 3 && s[1] == '-';
}


/**
 * @brief delta(curr_state, symbol) returns the next state
 * 
 * @param curr_state current state
 * @param symbol current input symbol
 * @return State representing the next state
 */
static State delta(const State &curr_state, const char symbol, const DFA &dfa) {
    State next_state{};

    if (symbol == ' ') {
        next_state.error = true;
        return next_state;
    }

    if (!char_in_vector(dfa.alphabet, symbol)) {
        next_state.error = true;
        return next_state;
    }

    for (const Transition &t : dfa.transitions) {
        if (curr_state.state_name == t.from_state && char_in_vector(t.symbols, symbol)) {
            next_state.state_name = t.to_state;
            next_state.accepting = is_state_accepting(dfa.states, next_state.state_name);
            next_state.error = false;
            return next_state;
        }
    }

    next_state.error = true;
    return next_state;
}


static DFA construct_dfa() {
    std::istringstream iss{ASSEMBLY_DFA};
    std::string dfa_line{};
    std::getline(iss, dfa_line); // Skip .ALPHABET

    std::vector<char> alphabet{};

    while (iss >> dfa_line) {
        if (dfa_line == STATES) break;
        else {
            if (is_char(dfa_line)) {
                alphabet.push_back(dfa_line[0]);
            } else if (is_range(dfa_line)) {
                for (int i{dfa_line[0]}; i <= dfa_line[2]; ++i) {
                    alphabet.push_back(i);
                }
            }
        }
    }

    std::getline(iss, dfa_line); // Skip the newline after .STATES
    
}


std::vector<Token> tokenize_line(const std::string &line) {

}
