// WLP64 Unified Command-Line Interface

// All commands are in the form of 
// wlp64 <command> <file>
// 
// where command is one of ['compile', 'assemble', 'run']
// and file is an input file.
//
// The input file must have file extension .wlp4, .asm, .bin
// depending on the command (.asm for assemble, .wlp4 for compile, and any for run).

#include "assembler/assembler.hpp"
#include "compiler/wlp4_compiler.hpp"
#include "emulator/cpu.hpp"

#include <set>


const std::set<std::string> valid_commands = {
    "compile", "assemble", "run"
};

const std::set<std::string> valid_extensions = {
    ".wlp4", ".asm", ".bin"
};

bool has_period(const std::string &s, std::size_t &idx) {
    bool found{false};

    for (std::size_t i{0}; i < s.length(); ++i) {
        if (s[i] == '.') {
            if (found) return false;
            idx = i;
            found = true;
        }
    }

    return found;
}

void validate_command_line_args(std::string &command, std::string &filename) {
    require(valid_commands.find(command) != valid_commands.end(), "ERROR: Invalid command");

    std::size_t dot_idx{0};
    std::string extension{};
    require(has_period(filename, dot_idx), "ERROR: Not a valid filename");

    for (std::size_t i{dot_idx}; i < filename.length(); ++i) {
        extension += filename[i];
    }

    require(valid_extensions.find(extension) != valid_extensions.end(), "ERROR: Invalid file extension");

    require(
        (command == "run") ||
        (command == "assemble" && extension == ".asm") ||
        (command == "compile" && extension == ".wlp4"),
        "ERROR: Invalid combination of command and file extension"
    );
}


int main(int argc, char *argv[]) {
    require(argc == 3, "ERROR: Usage must be 'wlp4 <command> <file>'");

    std::string command{argv[1]};
    std::string filename{argv[2]};

    validate_command_line_args(command, filename);

    ...
}