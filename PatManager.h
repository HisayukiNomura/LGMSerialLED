#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>

// PatManager: パターン配列を内部に保持し、ガンマ補正を上書き適用するシンプルな管理クラス。
// - フラット配列入力（0x00GGRRBB の 32bit ピクセル）。
// - 元データの保持は不要という要件のため、setGamma は内部作業バッファを上書きで適用。
// - 複数インスタンス生成可。
class PatManager {
public:
    PatManager() = default;

    // 初期化: フラット配列 srcFlat から count 個のパターン（各 width*height ピクセル）をコピーして内部保持。
    // 戻り値: 成功 true / 失敗 false（引数不正や確保失敗など）。
    bool init(const std::uint32_t* srcFlat,
              std::size_t count,
              std::uint16_t width,
              std::uint16_t height);

    // ガンマ補正を現在の内部バッファに上書き適用（G/R/B 各 8bit に同一ガンマを適用）。
    // gamma > 0 を想定。成功 true / 失敗 false（未初期化など）。
    bool setGamma(float gamma);

    // 指定パターンの先頭ポインタを返す（非 const）。範囲外は nullptr。
    std::uint32_t* getBufferPtr(std::size_t patternIndex);

    // 便宜的なゲッター
    inline std::size_t count() const { return count_; }
    inline std::uint16_t width() const { return width_; }
    inline std::uint16_t height() const { return height_; }

private:
    std::unique_ptr<std::uint32_t[]> buf_; // ガンマ適用後の作業バッファのみ保持
    std::size_t count_ { 0 };
    std::uint16_t width_ { 0 };
    std::uint16_t height_ { 0 };
};
