#pragma once

#include <cstdint>
#include <cstddef>

// LGMPat: 16x16 のパターンを並べた配列（各要素は 0x00GGRRBB）
extern const std::uint32_t ZELDALeft[][16 * 16];
extern const std::uint32_t ZELDARight[][16 * 16];
extern const std::uint32_t ZELDAFront[][16 * 16];
extern const std::uint32_t ZELDABack[][16 * 16];
extern const std::uint32_t ZELDAStay[16 * 16];

// LGMPat の総パターン数（定義側で sizeof から自動算出）
extern const std::size_t ZELDALeftCount;
extern const std::size_t ZELDARightCount;
extern const std::size_t ZELDAFrontCount;
extern const std::size_t ZELDABackCount;

// 歩く速度
extern const std::uint16_t iWaitZeldaWalk;
extern const std::uint16_t iWaitZeldaRun;

