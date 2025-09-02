#pragma once

#include <cstdint>
#include <cstddef>

extern const std::uint32_t KirbyStay[];
extern const std::uint32_t KirbyWalk[][16 * 16];
extern const std::uint32_t KirbyRoll[][16 * 16];

extern const std::size_t KirbyWalkCount;
extern const std::size_t KirbyRollcount;

extern const std::uint16_t iWaitKirbyWalk;
extern const std::uint16_t iWaitKirbyRun;
