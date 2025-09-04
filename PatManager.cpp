/**
 * @file PatManager.cpp
 * @brief パターン管理クラスの実装
 * @details パターン配列(0x00GGRRBB)を保持し、LUTベースで破壊的に変換する管理クラスの実装ファイル
 */

/**
 * @brief パターン管理（ガンマ/明度/コントラスト/各色レンジ補正）の実装。
 * @details フラット配列(0x00GGRRBB)を内部に保持し、LUTベースで破壊的に変換します。
 */
#include "PatManager.h"
#include "stdio.h"
#include <cmath>
#include <cstring>

namespace {
    /**
     * @brief ガンマ補正用のLUTを構築します
     * @param gamma ガンマ値（正の値）
     * @param lut 出力先のLUT配列（256要素）
     * @return なし
     * @details 0-255の入力値をガンマ補正により0-255の出力値に変換するLUTを作成します。
     *          gamma <= 0の場合は1.0にフォールバックします。
     */
    inline void build_gamma_lut(float gamma, std::uint8_t lut[256]) {

		if (gamma <= 0.0f) gamma = 1.0f; ///< フォールバック値
        const float inv = 1.0f / 255.0f; ///< 正規化係数
        for (int i = 0; i < 256; ++i) {
            float n = static_cast<float>(i) * inv;     ///< 0..1に正規化
            float g = std::pow(n, gamma);              ///< ガンマ補正適用
            int v = static_cast<int>(g * 255.0f + 0.5f); ///< 0..255に戻して四捨五入
            if (v < 0) v = 0; else if (v > 255) v = 255; ///< クリッピング
			//printf("lut[%d] = %d  (n=%f , g = %f) \n", i, static_cast<std::uint8_t>(v) , n , g);
			lut[i] = static_cast<std::uint8_t>(v);
        }
    }

    /**
     * @brief 単一ピクセルにガンマ補正LUTを適用します
     * @param c 入力ピクセル値（0x00GGRRBB形式）
     * @param lut ガンマ補正LUT配列
     * @return ガンマ補正済みピクセル値（0x00GGRRBB形式）
     * @details 0x00GGRRBB形式の各8ビットチャネルにLUTを適用し、補正済みピクセル値を返します。
     */
    inline std::uint32_t apply_gamma_pixel(std::uint32_t c, const std::uint8_t lut[256]) {
        std::uint8_t b = static_cast<std::uint8_t>(c & 0xFF);        ///< 青チャネル抽出
        std::uint8_t r = static_cast<std::uint8_t>((c >> 8) & 0xFF); ///< 赤チャネル抽出
        std::uint8_t g = static_cast<std::uint8_t>((c >> 16) & 0xFF); ///< 緑チャネル抽出
        std::uint8_t bb = lut[b]; ///< 青チャネル補正
        std::uint8_t rr = lut[r]; ///< 赤チャネル補正
        std::uint8_t gg = lut[g]; ///< 緑チャネル補正
        return (static_cast<std::uint32_t>(gg) << 16) |
               (static_cast<std::uint32_t>(rr) << 8)  |
               (static_cast<std::uint32_t>(bb));
    }

    /**
     * @brief 整数値を8ビット範囲にクリップします
     * @param v クリップ対象の整数値
     * @return クリップされた8ビット値（0-255）
     * @details 入力値が0未満なら0、255超過なら255を返し、それ以外はそのまま返します。
     */
    inline std::uint8_t clip_u8_int(int v) {
        if (v < 0) return 0;
        if (v > 255) return 255;
        return static_cast<std::uint8_t>(v);
    }

    /**
     * @brief 明度/コントラスト補正用の合成LUTを構築します
     * @param brightnessPercent 明度パーセント（-100..100）
     * @param contrastPercent コントラストパーセント（-100..100）
     * @param lut 出力先のLUT配列（256要素）
     * @return なし
     * @details コントラスト→明度の順で適用する合成LUTを作成します。
     *          範囲外の値は内部でクリップされます。
     *          手順: v' = (v - 128) * (1 + c/100) + 128 → v'' = v' + (255 * b/100)
     */
    inline void build_bc_lut(int brightnessPercent, int contrastPercent, std::uint8_t lut[256]) {
        // クリップ
        if (brightnessPercent < -100) brightnessPercent = -100;
        if (brightnessPercent > 100)  brightnessPercent = 100;
        if (contrastPercent < -100)   contrastPercent = -100;
        if (contrastPercent > 100)    contrastPercent = 100;

        const float c = 1.0f + static_cast<float>(contrastPercent) / 100.0f; ///< コントラスト係数
        const float b = static_cast<float>(brightnessPercent) / 100.0f;      ///< 明度係数
        const int add = static_cast<int>(std::round(255.0f * b));            ///< 明度加算値

        for (int v = 0; v <= 255; ++v) {
            float v1 = (static_cast<float>(v) - 128.0f) * c + 128.0f; ///< コントラスト適用
            int v2 = static_cast<int>(std::round(v1)) + add;          ///< 明度適用
            lut[v] = clip_u8_int(v2);
        }
    }

    /**
     * @brief チャネルレンジ圧縮用のLUTを構築します
     * @param minV 出力最小値
     * @param maxV 出力最大値
     * @param lut 出力先のLUT配列（256要素）
     * @return なし
     * @details v' = min + round((max-min) * v / 255) で変換しますが、v==0（黒）は0を維持します。
     */
    inline void build_range_lut(std::uint8_t minV, std::uint8_t maxV, std::uint8_t lut[256]) {
        int minI = static_cast<int>(minV); ///< 最小値（整数）
        int maxI = static_cast<int>(maxV); ///< 最大値（整数）
        int span = maxI - minI; ///< レンジ幅
        for (int v = 0; v <= 255; ++v) {
            if (v == 0) { ///< 黒は維持
                lut[v] = 0;
                continue;
            }
            int mapped = minI + static_cast<int>(std::lround((span * v) / 255.0)); ///< レンジマッピング
            if (mapped < 0) mapped = 0; else if (mapped > 255) mapped = 255;       ///< クリッピング
            lut[v] = static_cast<std::uint8_t>(mapped);
        }
    }

    /**
     * @brief 指定チャネルのみにレンジLUTを適用します
     * @param c 入力ピクセル値（0x00GGRRBB形式）
     * @param lut レンジ補正LUT配列
     * @param channel 対象チャネル（'g'=緑, 'r'=赤, 'b'=青）
     * @return チャネル補正済みピクセル値（0x00GGRRBB形式）
     * @details 0x00GGRRBB形式の指定されたチャネルのみにLUTを適用し、他のチャネルはそのまま保持します。
     */
    inline std::uint32_t apply_channel_range(std::uint32_t c, const std::uint8_t lut[256], char channel) {
        std::uint8_t b = static_cast<std::uint8_t>(c & 0xFF);        ///< 青チャネル抽出
        std::uint8_t r = static_cast<std::uint8_t>((c >> 8) & 0xFF); ///< 赤チャネル抽出
        std::uint8_t g = static_cast<std::uint8_t>((c >> 16) & 0xFF); ///< 緑チャネル抽出
        switch (channel) {
            case 'g': g = lut[g]; break; ///< 緑チャネル補正
            case 'r': r = lut[r]; break; ///< 赤チャネル補正
            case 'b': b = lut[b]; break; ///< 青チャネル補正
            default: break; ///< 無効なチャネル指定
        }
        return (static_cast<std::uint32_t>(g) << 16) |
               (static_cast<std::uint32_t>(r) << 8)  |
               (static_cast<std::uint32_t>(b));
    }
}

/**
 * @brief 内部バッファとメタデータを初期化します。
 * @return なし
 * @details 内部バッファを解放し、初期化フラグとすべてのメタデータ（サイズ情報）をリセットします。
 *          オブジェクトを初期状態に戻すために使用されます。
 */
void PatManager::reset()
{
	buf_.reset();
	isInitialized = false;
	count_ = 0;
	width_ = 0;
	height_ = 0;
}

/**
 * @brief デストラクタ。内部リソースを解放します。
 * @return なし
 * @details オブジェクト破棄時に内部バッファとメタデータを明示的に解放してクリーンアップを行います。
 */
PatManager::~PatManager()
{
    // 明示的に解放してクリーンアップ
	reset();
}

/**
 * @brief フラット配列から内部バッファにコピーして初期化します。
 * @param srcFlat 入力配列(0x00GGRRBB)
 * @param count パターン数
 * @param width 幅
 * @param height 高さ
 * @return 成功ならtrue
 * @details フラット配列からデータをコピーして内部バッファを初期化します。
 *          パラメータの妥当性をチェックし、メモリ確保を行います。
 *          既存のバッファは新しいデータで置き換えられます。
 */
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

/**
 * @brief ガンマ補正をLUTで適用します。
 * @param gamma ガンマ値(>0)
 * @return 成功ならtrue
 * @details LUTベースでガンマ補正を全ピクセルに適用します。
 *          gamma <= 0の場合は処理をスキップしてtrueを返します。
 *          すべてのチャネル（緑/赤/青）に同一のガンマ値が適用されます。
 */
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

/**
 * @brief 緑チャネルのレンジ圧縮を適用します。
 * @param minV 最小値
 * @param maxV 最大値
 * @return 成功ならtrue
 * @details 緑チャネルの値を指定された範囲にマッピングします。
 *          minV、maxVともに0の場合は処理をスキップしてtrueを返します。
 *          黒（0値）は維持され、その他の値は比例配分されます。
 */
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

/**
 * @brief 赤チャネルのレンジ圧縮を適用します。
 * @param minV 最小値
 * @param maxV 最大値
 * @return 成功ならtrue
 * @details 赤チャネルの値を指定された範囲にマッピングします。
 *          minV、maxVともに0の場合は処理をスキップしてtrueを返します。
 *          黒（0値）は維持され、その他の値は比例配分されます。
 */
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

/**
 * @brief 青チャネルのレンジ圧縮を適用します。
 * @param minV 最小値
 * @param maxV 最大値
 * @return 成功ならtrue
 * @details 青チャネルの値を指定された範囲にマッピングします。
 *          minV、maxVともに0の場合は処理をスキップしてtrueを返します。
 *          黒（0値）は維持され、その他の値は比例配分されます。
 */
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

/**
 * @brief 指定パターン先頭ポインタを返します。
 * @param patternIndex パターン番号
 * @return 先頭ポインタ（範囲外はnullptr）
 * @details 指定されたパターンインデックスに対応する内部バッファの先頭ポインタを返します。
 *          範囲外のインデックスやバッファが未初期化の場合はnullptrを返します。
 *          返されたポインタは直接操作可能ですが、サイズの境界は呼び出し側で管理してください。
 */
std::uint32_t* PatManager::getBufferPtr(std::size_t patternIndex)
{
    if (patternIndex >= count_ || !buf_) return nullptr;
    const std::size_t pixelsPerPat = static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_);
	return buf_.get() + patternIndex * pixelsPerPat;
}

/**
 * @brief 明度/コントラストをLUTで適用します（コントラスト→明度）。
 * @param brightnessPercent 明度(-100..100)
 * @param contrastPercent コントラスト(-100..100)
 * @return 成功ならtrue
 * @details コントラスト補正に続いて明度補正を適用します。
 *          範囲外の値は内部でクリップされます。
 *          合成LUTを使用して効率的に処理されます。
 */
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
