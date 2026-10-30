#include <array>
#include <cstdint>
#include <vector>

struct CPU {
    std::array<std::uint64_t, 31> registers{};

    std::uint64_t pc{0};
    std::uint64_t sp{0};
    const std::uint64_t xzr{0};
    std::uint32_t instruction{0};

    bool n{false};
    bool z{false};
    bool c{false};
    bool v{false};

    std::vector<std::uint8_t> memory;

    bool halted{false};
};
