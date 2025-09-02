#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "pico/time.h"

/**
 * @brief WS2812(NeoPixel) を RP2040 の PIO で駆動するためのユーティリティクラス。
 * @details
 * - PIO ステートマシンで 1-wire プロトコルを生成し、VRAM(0x00GGRRBB)からフレームを送出します。
 * - 走査（serpentine/leftToRight）やパネル分割に対応し、簡易描画ヘルパーも提供します。
 */
class WS2812 {
				uint8_t m_pin;      ///< データ出力GPIO
				PIO m_pio;          ///< 使用するPIOインスタンス
				uint m_sm;          ///< ステートマシン番号
				int m_offset;       ///< PIOプログラムのロードオフセット

				public:
					uint32_t* pVRam;  ///< VRAM（GRB 24bit、1要素=1ピクセル）
					uint32_t xVRam;   ///< VRAMの幅（ピクセル）
					uint32_t yVRam;   ///< VRAMの高さ（ピクセル）

				public:
					// １枚のパネルサイズと、そのパネルが複数枚ある場合の数。パネルのカスケード順は左上から右下に固定とする
					uint8_t xSize;        ///< 1パネルの横ピクセル数
					uint8_t ySize;        ///< 1パネルの縦ピクセル数
					uint8_t xPanelCount;  ///< 水平方向のパネル枚数
					uint8_t yPanelCount;  ///< 垂直方向のパネル枚数

				public:
					/**
					 * @brief ドライバを構築・初期化します。
					 * @param pin データ出力GPIO
					 * @param a_xSize 1枚のパネルの幅（ピクセル）
					 * @param a_ySize 1枚のパネルの高さ（ピクセル）
					 * @param a_xPanelCount パネルの水平方向枚数
					 * @param a_yPanelCount パネルの垂直方向枚数
					 * @return なし
					 * @details PIOへプログラムをロードし、800kHz相当でSMを初期化。VRAMを確保します。
					 */
					WS2812(uint8_t pin, uint8_t a_xSize, uint8_t a_ySize, uint8_t a_xPanelCount = 1, uint8_t a_yPanelCount = 1);

					/** @brief リセットラッチ用 Low パルスを出力します。 @return なし @details フレーム送出前に呼び出してください。 */
					void Reset();
					/** @brief アイドル時に High を維持します。 @return なし @details PIOのidleループへ遷移します。 */
					void Keep();
					/** @brief 1ピクセルを即時送信します。 @param r 赤 @param g 緑 @param b 青 @return なし @details VRAMを使わずブロッキング送信。 */
					void setColorDirect(uint8_t r, uint8_t g, uint8_t b);
					/** @brief 24bit GRB値を即時送信します。 @param c 0x00GGRRBB @return なし */
					void setColorDirect(uint32_t c);
					/** @brief デモ用にランダム色を1ピクセル送信します。 @return なし */
					void fillRandomColorDirect();

					// 画面の更新
					// serpentine=true の場合、行ごとに左右が反転。leftToRight は偶数行(行0,2,...)の基準方向。
					/** @brief 1パネル分を送信します。 @param posX パネル左上X @param posY パネル左上Y @param serpentine 千鳥配線 @param leftToRight 偶数行の基準方向 @return なし */
					void ScanPanel(uint8_t posX, uint8_t posY, bool serpentine = false, bool leftToRight = true);
					/** @brief 全パネルを送信します。 @param serpentine 千鳥配線 @param leftToRight 偶数行の基準方向 @return なし */
					void ScanBuffer(bool serpentine = false, bool leftToRight = true);

					// VRAM 操作用のユーティリティ
					/** @brief VRAMを指定色で塗りつぶします。 @param rgb 0x00GGRRBB @return なし */
					void Clear(uint32_t rgb = 0);
					/** @brief VRAMの1ピクセルを書き換えます。 @param x X @param y Y @param rgb 0x00GGRRBB @return なし */
					void SetPixel(uint16_t x, uint16_t y, uint32_t rgb);
         
					// 各パネルの外枠に色を描く
					/** @brief 指定パネルの外枠を描画します。 @param panelX Xインデックス @param panelY Yインデックス @param rgb 0x00GGRRBB @return なし */
					void DrawPanelBorder(uint8_t panelX, uint8_t panelY, uint32_t rgb);
					/** @brief すべてのパネルにランダム外枠を描画します。 @return なし */
					void DrawRandomBorders();

					/**
					 * @brief 任意のパターン配列をVRAMへ描画します。
					 * @param pattern 0x00GGRRBB のフラット配列
					 * @param width パターン幅
					 * @param height パターン高さ
					 * @param X 貼り付け先 左上X
					 * @param y 貼り付け先 左上Y
					 * @param colorReplace 置換色（0で無効）
					 * @param isOverlay 黒(0)を透明として重ねる
					 * @return なし
					 */
					void DrawBuffer(const uint32_t pattern[],uint8_t width , uint8_t height, uint8_t X, uint8_t y,uint32_t colorReplace , bool isOverlay);
};
