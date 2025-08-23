#include <iostream>
#include <cmath>
#include <unordered_map>
#include <cstdint>
#include <iomanip>
#include <algorithm> // std::hex, std::setw, std::setfill
#include "GammaCorrector.h"

uint32_t GammaCorrector::correct(uint32_t grbColor)
{
	// キャッシュに存在すれば再利用
	auto it = cache_.find(grbColor);
	if (it != cache_.end()) return it->second;

	// GRBから各成分抽出
	uint8_t g = (grbColor >> 16) & 0xFF;
	uint8_t r = (grbColor >> 8) & 0xFF;
	uint8_t b = grbColor & 0xFF;

	// ガンマ補正
	uint8_t g_corr = gammaCorrect(g);
	uint8_t r_corr = gammaCorrect(r);
	uint8_t b_corr = gammaCorrect(b);

	// 補正後のGRBを再構成
	uint32_t corrected = (g_corr << 16) | (r_corr << 8) | b_corr;

	// キャッシュに保存
	cache_[grbColor] = corrected;
	return corrected;
}

uint8_t GammaCorrector::gammaCorrect(uint8_t value) const {
	float normalized = static_cast<float>(value) / 255.0f;
	float corrected = std::pow(normalized, 1.0f / gamma_);
	return static_cast<uint8_t>(std::clamp(corrected * 255.0f, 0.0f, 255.0f));
}
