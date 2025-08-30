#include "PatManager.h"
#include "stdio.h"
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
			//printf("lut[%d] = %d  (n=%f , g = %f) \n", i, static_cast<std::uint8_t>(v) , n , g);
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

    inline std::uint8_t clip_u8_int(int v) {
        if (v < 0) return 0;
        if (v > 255) return 255;
        return static_cast<std::uint8_t>(v);
    }

    // 明度/コントラストの合成LUTを作る
    // contrast: -100..100, brightness: -100..100
    // 手順: v' = (v - 128) * (1 + c/100) + 128 → v'' = v' + (255 * b/100)
    inline void build_bc_lut(int brightnessPercent, int contrastPercent, std::uint8_t lut[256]) {
        // クリップ
        if (brightnessPercent < -100) brightnessPercent = -100;
        if (brightnessPercent > 100)  brightnessPercent = 100;
        if (contrastPercent < -100)   contrastPercent = -100;
        if (contrastPercent > 100)    contrastPercent = 100;

        const float c = 1.0f + static_cast<float>(contrastPercent) / 100.0f;
        const float b = static_cast<float>(brightnessPercent) / 100.0f;
        const int add = static_cast<int>(std::round(255.0f * b));

        for (int v = 0; v <= 255; ++v) {
            float v1 = (static_cast<float>(v) - 128.0f) * c + 128.0f; // コントラスト
            int v2 = static_cast<int>(std::round(v1)) + add;          // 明度
            lut[v] = clip_u8_int(v2);
        }
    }

    // v' = min + round((max-min) * v / 255) ただし v==0 は 0 を維持
    inline void build_range_lut(std::uint8_t minV, std::uint8_t maxV, std::uint8_t lut[256]) {
        int minI = static_cast<int>(minV);
        int maxI = static_cast<int>(maxV);
        int span = maxI - minI; // 入力検証は不要という仕様
        for (int v = 0; v <= 255; ++v) {
            if (v == 0) { // 黒は維持
                lut[v] = 0;
                continue;
            }
            int mapped = minI + static_cast<int>(std::lround((span * v) / 255.0));
            if (mapped < 0) mapped = 0; else if (mapped > 255) mapped = 255;
            lut[v] = static_cast<std::uint8_t>(mapped);
        }
    }

    // 指定チャネル（'g','r','b'）のみに range LUT を適用（0x00GGRRBB）
    inline std::uint32_t apply_channel_range(std::uint32_t c, const std::uint8_t lut[256], char channel) {
        std::uint8_t b = static_cast<std::uint8_t>(c & 0xFF);
        std::uint8_t r = static_cast<std::uint8_t>((c >> 8) & 0xFF);
        std::uint8_t g = static_cast<std::uint8_t>((c >> 16) & 0xFF);
        switch (channel) {
            case 'g': g = lut[g]; break;
            case 'r': r = lut[r]; break;
            case 'b': b = lut[b]; break;
            default: break;
        }
        return (static_cast<std::uint32_t>(g) << 16) |
               (static_cast<std::uint32_t>(r) << 8)  |
               (static_cast<std::uint32_t>(b));
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
    isInitialized = true;
	return true;
}

bool PatManager::setGamma(float gamma)
{
	if (gamma <= 0.0f) return true;
	if (!buf_ || count_ == 0 || width_ == 0 || height_ == 0) return false;

    std::uint8_t lut[256];
    build_gamma_lut(gamma, lut);

    const std::size_t total = static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_) * count_;
    for (std::size_t i = 0; i < total; ++i) buf_[i] = apply_gamma_pixel(buf_[i], lut);
    return true;
}

bool PatManager::setGreenRange(std::uint8_t minV, std::uint8_t maxV)
{
	if (minV == 0 && maxV == 0) return true;
	if (!buf_ || count_ == 0 || width_ == 0 || height_ == 0) return false;
    std::uint8_t lut[256];
    build_range_lut(minV, maxV, lut);
    const std::size_t total = static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_) * count_;
    for (std::size_t i = 0; i < total; ++i) buf_[i] = apply_channel_range(buf_[i], lut, 'g');
    return true;
}

bool PatManager::setRedRange(std::uint8_t minV, std::uint8_t maxV)
{
	if (minV == 0 && maxV == 0) return true;
    if (!buf_ || count_ == 0 || width_ == 0 || height_ == 0) return false;
    std::uint8_t lut[256];
    build_range_lut(minV, maxV, lut);
    const std::size_t total = static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_) * count_;
    for (std::size_t i = 0; i < total; ++i) buf_[i] = apply_channel_range(buf_[i], lut, 'r');
    return true;
}

bool PatManager::setBlueRange(std::uint8_t minV, std::uint8_t maxV)
{
	if (minV == 0 && maxV == 0) return true;
	if (!buf_ || count_ == 0 || width_ == 0 || height_ == 0) return false;
	std::uint8_t lut[256];
    build_range_lut(minV, maxV, lut);
    const std::size_t total = static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_) * count_;
    for (std::size_t i = 0; i < total; ++i) buf_[i] = apply_channel_range(buf_[i], lut, 'b');
    return true;
}

std::uint32_t* PatManager::getBufferPtr(std::size_t patternIndex)
{
    if (patternIndex >= count_ || !buf_) return nullptr;
    const std::size_t pixelsPerPat = static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_);
	return buf_.get() + patternIndex * pixelsPerPat;
}

bool PatManager::setBrightnessContrast(int brightnessPercent, int contrastPercent)
{

    if (!buf_ || count_ == 0 || width_ == 0 || height_ == 0) return false;

    std::uint8_t lut[256];
    build_bc_lut(brightnessPercent, contrastPercent, lut);

    const std::size_t total = static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_) * count_;
    for (std::size_t i = 0; i < total; ++i) {
        buf_[i] = apply_gamma_pixel(buf_[i], lut); // LUTは線形変換でも流用できる
    }
    return true;
}
