#include "PatManager.h"

#include <cmath>
#include <cstring>

namespace {
    // 0..255 の入力を gamma に従って補正した 0..255 を返す LUT を作成
    inline void build_gamma_lut(float gamma, std::uint8_t lut[256]) {
        if (gamma <= 0.0f) gamma = 1.0f; // フォールバック
        const float inv = 1.0f / 255.0f;
        for (int i = 0; i < 256; ++i) {
            float n = static_cast<float>(i) * inv;     // 0..1
            float g = std::pow(n, gamma);              // 補正
            int v = static_cast<int>(g * 255.0f + 0.5f);
            if (v < 0) v = 0; else if (v > 255) v = 255;
            lut[i] = static_cast<std::uint8_t>(v);
        }
    }

    // 0x00GGRRBB の各 8bit に LUT を適用
    inline std::uint32_t apply_gamma_pixel(std::uint32_t c, const std::uint8_t lut[256]) {
        std::uint8_t b = static_cast<std::uint8_t>(c & 0xFF);
        std::uint8_t r = static_cast<std::uint8_t>((c >> 8) & 0xFF);
        std::uint8_t g = static_cast<std::uint8_t>((c >> 16) & 0xFF);
        std::uint8_t bb = lut[b];
        std::uint8_t rr = lut[r];
        std::uint8_t gg = lut[g];
        return (static_cast<std::uint32_t>(gg) << 16) |
               (static_cast<std::uint32_t>(rr) << 8)  |
               (static_cast<std::uint32_t>(bb));
    }
}

bool PatManager::init(const std::uint32_t* srcFlat,
                      std::size_t count,
                      std::uint16_t width,
                      std::uint16_t height)
{
    if (!srcFlat || count == 0 || width == 0 || height == 0) return false;

    const std::size_t pixelsPerPat = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
    const std::size_t total = pixelsPerPat * count;

    // メモリ確保（既存を置き換え）
    std::unique_ptr<std::uint32_t[]> tmp(new (std::nothrow) std::uint32_t[total]);
    if (!tmp) return false;

    std::memcpy(tmp.get(), srcFlat, total * sizeof(std::uint32_t));

    buf_ = std::move(tmp);
    count_ = count;
    width_ = width;
    height_ = height;
    return true;
}

bool PatManager::setGamma(float gamma)
{
    if (!buf_ || count_ == 0 || width_ == 0 || height_ == 0) return false;

    std::uint8_t lut[256];
    build_gamma_lut(gamma, lut);

    const std::size_t total = static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_) * count_;
    for (std::size_t i = 0; i < total; ++i) buf_[i] = apply_gamma_pixel(buf_[i], lut);
    return true;
}

std::uint32_t* PatManager::getBufferPtr(std::size_t patternIndex)
{
    if (patternIndex >= count_ || !buf_) return nullptr;
    const std::size_t pixelsPerPat = static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_);
    return buf_.get() + patternIndex * pixelsPerPat;
}
