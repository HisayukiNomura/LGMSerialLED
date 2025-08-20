#pragma once

#include <cstdint>
#include <cstddef>

// LGMPat: 16x16 のパターンを並べた配列（各要素は 0x00GGRRBB）
extern const std::uint32_t LGMPat[][16 * 16];
extern const std::uint32_t LGMRed[];
// LGMPat の総パターン数（定義側で sizeof から自動算出）
extern const std::size_t LGMPatCount;
