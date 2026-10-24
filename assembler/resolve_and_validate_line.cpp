#include "assembler.hpp"


// Given a general-purpose register as a string (x30), returns the number portion as an integer
int read_reg(const std::string &reg) {
    int number{std::stoi(reg.substr(1))};

    require(
        number >= 0 && number <= 30,
        "ERROR: Invalid register"
    );

    return number;
}