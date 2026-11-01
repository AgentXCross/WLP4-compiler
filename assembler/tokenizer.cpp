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

std::pair<char, char> range_start_and_end(const std::string s) {
    if (!is_range(s)) format_error("ERROR: Can't call range_start_and_end on a string that isn't a range");

    return {s[0], s[2]};
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

/**
 * @brief Constructs the DFA (Deterministic Finite Automaton) to recognize ARM64 tokens
 */
static DFA construct_dfa() {
    std::istringstream iss{ASSEMBLY_DFA};
    std::string dfa_line{};
    std::getline(iss, dfa_line); // Skip .ALPHABET

    std::vector<char> alphabet{};

    while (std::getline(iss, dfa_line)) {
        if (dfa_line == STATES) break;

        if (is_char(dfa_line)) {
            alphabet.push_back(dfa_line[0]);
        } else if (is_range(dfa_line)) {
            std::pair<char, char> range{range_start_and_end(dfa_line)};
            for (char i{range.first}; i <= range.second; ++i) {
                alphabet.push_back(i);
            }
        }
    }

    std::vector<State> states{};
    std::string initial_state{};

    while (std::getline(iss, dfa_line)) {
        if (dfa_line == TRANSITIONS) break;

        bool accepting{false};

        if (dfa_line.back() == '!' && !is_char(dfa_line)) {
            accepting = true;
            dfa_line.pop_back();
        }

        if (initial_state == "") {
            if (accepting == true) {
                format_error("ERROR: Initial state can't be accepting");
            }

            initial_state = dfa_line;
        }

        states.push_back({dfa_line, accepting});
    }

    std::vector<Transition> transitions{};

    while (std::getline(iss, dfa_line)) {
        if (dfa_line == INPUT) break;

        std::string from_state{}, symbols{}, to_state{};

        std::istringstream iss{dfa_line};
        std::string s{};
        std::vector<std::string> line_vec{};

        while (iss >> s) {
            line_vec.push_back(s);
        }

        from_state = line_vec.front();
        to_state = line_vec.back();

        for (std::size_t i{1}; i < line_vec.size() - 1; ++i) {
            if (is_char(line_vec[i])) {
                symbols += line_vec[i];
            } else if (is_range(line_vec[i])) {
                std::pair<char, char> range{range_start_and_end(line_vec[i])};

                for (char c{range.first}; c <= range.second; ++c) {
                    symbols += c;
                }
            } else {
                format_error("ERROR: Invalid transition line in DFA");
            }
        }

        transitions.push_back({
            from_state,
            std::vector<char>{symbols.begin(), symbols.end()}, 
            to_state
        });
    }

    return DFA{alphabet, states, transitions, initial_state};
}


std::vector<Token> ARM64Assembler::tokenize_line(const std::string &line) {
    const std::size_t k{line.length()};

    static const DFA dfa{construct_dfa()};

    State curr{dfa.initial_state, false, false};

    std::string lexeme{};

    std::vector<Token> tokens{};

    for (std::size_t i{0}; i <= k; ) {
        // skip preceding whitespaces
        if (i < k && curr.state_name == dfa.initial_state && line[i] == ' ') {
            ++i;
            continue;
        }

        State next{};

        if (i == k) {
            if (!lexeme.empty()) {
                require(curr.accepting, "ERROR: Invalid assembly line");
                tokens.push_back({return_kind(curr.state_name), lexeme});
            }

            break;
        } else next = delta(curr, line[i], dfa);

        if (next.error == true) {
            require(curr.accepting, "ERROR: Invalid assembly line");

            tokens.push_back({return_kind(curr.state_name), lexeme});
            
            lexeme = "";
            curr.state_name = dfa.initial_state;
            curr.error = false;
            curr.accepting = false;
        } else {
            lexeme += line[i];
            curr = next;
            ++i;
        }
    }

    return tokens;
}
