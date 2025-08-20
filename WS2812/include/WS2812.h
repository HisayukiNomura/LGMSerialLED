#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "pico/time.h"

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
		  WS2812(uint8_t pin, uint8_t a_xSize, uint8_t a_ySize, uint8_t a_xPanelCount = 1, uint8_t a_yPanelCount = 1);
		  void Reset();
		  void Keep();
		  void setColorDirect(uint8_t r, uint8_t g, uint8_t b);
		  void setColorDirect(uint32_t c);
		  void fillRandomColorDirect();

		  // 画面の更新
		  // serpentine=true の場合、行ごとに左右が反転。leftToRight は偶数行(行0,2,...)の基準方向。
		  void ScanPanel(uint8_t posX, uint8_t posY, bool serpentine = false, bool leftToRight = true);
		  void ScanBuffer(bool serpentine = false, bool leftToRight = true);

		  // VRAM 操作用のユーティリティ
		  void Clear(uint32_t rgb = 0);
		  void SetPixel(uint16_t x, uint16_t y, uint32_t rgb);
		 
		  // 各パネルの外枠に色を描く
		  void DrawPanelBorder(uint8_t panelX, uint8_t panelY, uint32_t rgb);
		  void DrawRandomBorders();

		  void DrawBuffer(const uint32_t pattern[],uint8_t width , uint8_t height, uint8_t X, uint8_t y,uint32_t colorReplace , bool isOverlay);
};
