#include <string>
#include <vector>


// DFA Section markers (For ASSEMBLY_DFA)
const std::string ALPHABET      = ".ALPHABET";
const std::string STATES        = ".STATES";
const std::string TRANSITIONS   = ".TRANSITIONS";
const std::string INPUT         = ".INPUT";
const std::string EMPTY         = ".EMPTY";

struct State {
    std::string state_name;
    bool accepting = false;
    bool error = false;
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
    std::string initial_state;
};


const std::string ASSEMBLY_DFA =
    std::string{".ALPHABET\n"} +
    "0-9\n" +
    ".\n" +
    ",\n" +
    "[\n" +
    "]\n" +
    "-\n" +
    "a-z\n" +
    "A-Z\n" +
    ":\n" +
    ".STATES\n" +
    "start\n" +
    "dot\n" +
    "dot_alpha!\n" +
    "x!\n" +
    "xz!\n" +
    "xzr!\n" +
    "x_one_digit!\n" +
    "x_two_digits!\n" +
    "s!\n" +
    "sp!\n" +
    "0!\n" +
    "0x\n" +
    "0x_hex!\n" +
    "first_letter!\n" +
    "first_letter_then_alpha!\n" +
    "colon!\n" +
    "negative_sign\n" +
    "negative_sign_then_digits!\n" +
    "first1-9!\n" +
    "first1-9_then_digits!\n" +
    "comma!\n" +
    "left_bracket!\n" +
    "right_bracket!\n" +
    ".TRANSITIONS\n" +
    "start . dot\n" +
    "start ] right_bracket\n" +
    "start [ left_bracket\n" +
    "start , comma\n" +
    "start - negative_sign\n" +
    "start x x\n" +
    "start s s\n" +
    "start a-r t-w y z A-Z first_letter\n" +
    "start 0 0\n" +
    "start 1-9 first1-9\n" +
    "dot 0-9 a-z A-Z dot_alpha\n" +
    "dot_alpha 0-9 a-z A-Z dot_alpha\n" +
    "x z xz\n" +
    "x 0-9 x_one_digit\n" +
    "x a-y A-Z first_letter_then_alpha\n" +
    "x : colon\n" +
    "x_one_digit 0-9 x_two_digits\n" +
    "x_one_digit a-z A-Z first_letter_then_alpha\n" +
    "x_one_digit : colon\n" +
    "x_two_digits 0-9 a-z A-Z first_letter_then_alpha\n" +
    "x_two_digits : colon\n" +
    "xz r xzr\n" +
    "xz a-q s-z A-Z 0-9 first_letter_then_alpha\n" +
    "xz : colon\n" +
    "xzr a-z A-Z 0-9 first_letter_then_alpha\n" +
    "xzr : colon\n" +
    "s p sp\n" +
    "s 0-9 a-o q-z A-Z first_letter_then_alpha\n" +
    "s : colon\n" +
    "sp 0-9 a-z A-Z first_letter_then_alpha\n" +
    "sp : colon\n" +
    "first_letter 0-9 a-z A-Z first_letter_then_alpha\n" +
    "first_letter : colon\n" +
    "first_letter_then_alpha 0-9 a-z A-Z first_letter_then_alpha\n" +
    "first_letter_then_alpha : colon\n" +
    "0 x 0x\n" +
    "0x 0-9 a-f A-F 0x_hex\n" +
    "0x_hex 0-9 a-f A-F 0x_hex\n" +
    "first1-9 0-9 first1-9_then_digits\n" +
    "first1-9_then_digits 0-9 first1-9_then_digits\n" +
    "negative_sign 1-9 negative_sign_then_digits\n" +
    "negative_sign_then_digits 0-9 negative_sign_then_digits\n" +
    ".INPUT\n";