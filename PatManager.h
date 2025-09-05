/**
 * @file PatManager.h
 * @brief パターン管理クラスの定義
 * @details パターン配列(0x00GGRRBB)を保持し、各種補正を上書き適用する管理クラスのヘッダファイル
 */

#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>

/**
 * @brief パターン配列(0x00GGRRBB)を保持し、各種補正を上書き適用する管理クラス。
 * @details
 * - フラット配列を内部バッファにコピーして保持し、その上で LUT ベースの変換（ガンマ/明度/コントラスト/各色レンジ）を累積適用します。
 * - 元データを保持しない要件のため、すべて破壊的（in-place）に更新します。
 * - 複数インスタンスに対応。
 */
class PatManager {
public:
    bool isInitialized; ///< 初期化済みフラグ
public:
    /** @brief 既定コンストラクタ。@details フラグは未初期化(false)。 */
    PatManager() { isInitialized = false; }
    /** @brief デストラクタ。@details 内部バッファを解放します。 */
    ~PatManager();
    /** @brief 内部状態を解放/初期化します。@return なし */
    void reset();
    
    /**
     * @brief フラット配列から内部バッファを確保・コピーします。
     * @param srcFlat 0x00GGRRBB のフラット配列
     * @param count パターン数
     * @param width 幅(ピクセル)
     * @param height 高さ(ピクセル)
     * @return 成功ならtrue
     * @details フラット配列から内部バッファにデータをコピーして初期化します。
     *          既存のバッファは置き換えられます。パラメータの妥当性チェックを行い、
     *          メモリ確保に失敗した場合はfalseを返します。
     */
    bool init(const std::uint32_t* srcFlat,
              std::size_t count,
              std::uint16_t width,
              std::uint16_t height);

    /**
     * @brief ガンマ補正を上書き適用します（G/R/B同一ガンマ）。
     * @param gamma ガンマ値(>0)
     * @return 成功ならtrue
     * @details すべてのチャネル（緑/赤/青）に同一のガンマ値を適用します。
     *          LUTベースで効率的に処理され、既存データを上書きします。
     *          gamma <= 0の場合は処理をスキップしてtrueを返します。
     */
    bool setGamma(float gamma);

    /**
     * @brief 明度/コントラスト補正を上書き適用します。
     * @param brightnessPercent 明度(-100..100)
     * @param contrastPercent コントラスト(-100..100)
     * @return 成功ならtrue
     * @details 適用順は コントラスト→明度。範囲外は内部でクリップします。
     */
    bool setBrightnessContrast(int brightnessPercent, int contrastPercent);

    /**
     * @brief 緑チャネルのレンジ圧縮を適用します。
     * @param minV 最小値（デフォルト: 0）
     * @param maxV 最大値（デフォルト: 255）
     * @return 成功ならtrue
     * @details 緑チャネルの値を指定された範囲にマッピングします。
     *          黒（0値）は維持され、その他の値は比例配分されます。
     */
    bool setGreenRange(std::uint8_t minV = 0, std::uint8_t maxV = 255);
    
    /**
     * @brief 赤チャネルのレンジ圧縮を適用します。
     * @param minV 最小値（デフォルト: 0）
     * @param maxV 最大値（デフォルト: 255）
     * @return 成功ならtrue
     * @details 赤チャネルの値を指定された範囲にマッピングします。
     *          黒（0値）は維持され、その他の値は比例配分されます。
     */
    bool setRedRange  (std::uint8_t minV = 0, std::uint8_t maxV = 255);
    
    /**
     * @brief 青チャネルのレンジ圧縮を適用します。
     * @param minV 最小値（デフォルト: 0）
     * @param maxV 最大値（デフォルト: 255）
     * @return 成功ならtrue
     * @details 青チャネルの値を指定された範囲にマッピングします。
     *          黒（0値）は維持され、その他の値は比例配分されます。
     */
    bool setBlueRange (std::uint8_t minV = 0, std::uint8_t maxV = 255);

    /**
     * @brief 指定パターンの先頭アドレスを返します。
     * @param patternIndex 取得するパターン番号
     * @return 先頭ポインタ（範囲外は nullptr）
     * @details 指定されたパターンインデックスに対応する内部バッファの先頭ポインタを返します。
     *          範囲外のインデックスやバッファが未初期化の場合はnullptrを返します。
     */
    std::uint32_t* getBufferPtr(std::size_t patternIndex);

    // 便宜的なゲッター
    inline std::size_t count() const { return count_; }      ///< 保持パターン数
    inline std::uint16_t width() const { return width_; }    ///< パターン幅
    inline std::uint16_t height() const { return height_; }  ///< パターン高

private:
    std::unique_ptr<std::uint32_t[]> buf_; ///< 内部作業バッファ（0x00GGRRBB）
    std::size_t count_ { 0 };              ///< パターン数
    std::uint16_t width_ { 0 };            ///< 幅
    std::uint16_t height_ { 0 };           ///< 高さ
};
