#include <stdio.h>
#include <cstdint>
#include <array>
#include "pico/stdlib.h"
#include "hardware/clocks.h" // set_sys_clock_khz
#include "./WS2812/include/WS2812.h"
#include "pico/time.h"
#define PIN_WS2812_1 22 // GPIO 22 (29pin)
#define BUTTON_PIN 28    // GP28 をプルアップ入力で使用（スイッチ）
#include "LGMPat.h"     // パターン配列とカウント
#include "./WS2812/include/GammaCorrector.h"

static volatile uint8_t timer_count = 0;
int iState = 0; // 0が開始、1が停止状態、2が歩き、3が走り

// デバウンス付き：押下（プルアップなので Active-Low）を1回だけ検出
static bool button_pressed()
{
	static bool last_reading = true; // 初期は未押下(High)
	static bool debounced = true;
	static uint32_t last_change_ms = 0;
	static bool armed = true; // 押下1回につき1度だけtrueを返す

	bool reading = gpio_get(BUTTON_PIN);
	uint32_t now_ms = to_ms_since_boot(get_absolute_time());

	if (reading != last_reading) {
		last_change_ms = now_ms; // 変化を検出、デバウンス計測開始
		last_reading = reading;
	}

	// 30ms安定したら確定
	if ((now_ms - last_change_ms) > 30) {
		if (debounced != reading) {
			debounced = reading;
			if (debounced == false) {
				// 立下り＝押下確定（Active-Low）
				if (armed) {
					armed = false; // 次の解放まで無効化
					return true;
				}
			} else {
				// 解放で再武装
				armed = true;
			}
		}
	}
	return false;
}

bool one_shot_cb(repeating_timer_t* rt)
{
	(void)rt;
	timer_count++;
	if (timer_count >= 6) {
		timer_count = 0;
		iState = 0;
		return false; // タイマーは停止
	}
	return true;
}




int main()
{
	// WS2812 のタイミング定数が 125MHz 前提のため、起動直後に 125MHz に固定
	set_sys_clock_khz(125000, true);

	stdio_init_all();
	// 1枚 8x8 パネルを前提（必要に応じて枚数を変更）
	WS2812 led_matrix(PIN_WS2812_1, 16, 16);

	// ボタン(GP28)をプルアップ入力で初期化
	gpio_init(BUTTON_PIN);
	gpio_pull_up(BUTTON_PIN);
	gpio_set_dir(BUTTON_PIN, GPIO_IN);

	led_matrix.Reset();
	led_matrix.Clear(0);
	led_matrix.ScanBuffer();
	int currPatNo = 0;
	int prevPatNo = 0;

	// 時刻開始（マイクロ秒単位）
	uint64_t start_us = time_us_64();
	repeating_timer_t rt;


	while (true) {
		if (iState == 0) {
			led_matrix.Reset();
			led_matrix.DrawBuffer(LGMRed, 16, 16, 0, 0, 0x000700, false); // パターンを描画
			led_matrix.ScanBuffer(true, false);
			iState = 1;
		}
		else if (iState == 1)
		{
			// ボタンが押されたら、１０秒間隔のタイマーを開始する。
			if (button_pressed()) {
				// 時刻開始（マイクロ秒単位）
				// １０秒間隔タイマー
				if (!add_repeating_timer_ms(10000, one_shot_cb, NULL, &rt)) {
					printf("Failed to add timer\n");
					return 1;
				}
				iState = 2;
			}
		}
		else if (iState == 2 || iState == 3)
		{
			int iWaitMs;
			int iTransMs;
			if (timer_count < 5){
				iWaitMs = 120;
				iTransMs = iWaitMs / 4;
			} else {
				iWaitMs = 30;
				iTransMs = 5;
			}
			// 一つ前のパターンを暗く表示
			led_matrix.Reset();
			led_matrix.DrawBuffer(LGMPat[prevPatNo], 16, 16, 0, 0, 0x030000, true); // パターンを描画 (オーバーレイで短い時間を表示)
			led_matrix.DrawBuffer(LGMPat[currPatNo], 16, 16, 0, 0, 0x060000, true); // パターンを描画 (オーバーレイで短い時間を表示)
			led_matrix.ScanBuffer(true, false);
			sleep_ms(iTransMs);

			led_matrix.Reset();
			led_matrix.DrawBuffer(LGMPat[currPatNo], 16, 16, 0, 0, 0x070000, false); // パターンを描画
			led_matrix.ScanBuffer(true, false);

			// led_matrix.Keep();
			sleep_ms(iWaitMs);
			prevPatNo = currPatNo;                              // 前のパターン番号を保存
			if (++currPatNo >= static_cast<int>(LGMPatCount)) { // パターン番号設定
				currPatNo = 0;                                  // パターン番号をリセット
			}
		}
	}
}
