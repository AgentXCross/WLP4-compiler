

struct CPU {
    std::uint64_t registers[31]{};
    std::uint64_t pc{0};
    bool n{false};
    bool z{false};
    bool c{false};
    bool v{false};
};