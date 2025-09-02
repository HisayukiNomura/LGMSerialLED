#pragma once

#include <cstdint>
#include <cstddef>

// LGMPat: 16x16 のパターンを並べた配列（各要素は 0x00GGRRBB）
extern const std::uint32_t DQ3Left[][16 * 16];
extern const std::uint32_t DQ3Right[][16 * 16];
extern const std::uint32_t DQ3Front[][16 * 16];
extern const std::uint32_t DQ3Back[][16 * 16];
extern const std::uint32_t DQ3Stay[16 * 16];

// LGMPat の総パターン数（定義側で sizeof から自動算出）
extern const std::size_t DQ3LeftCount;
extern const std::size_t DQ3RightCount;
extern const std::size_t DQ3FrontCount;
extern const std::size_t DQ3BackCount;

// 歩く速度
extern const std::uint16_t iWaitDQ3Walk;
extern const std::uint16_t iWaitDQ3Run;

