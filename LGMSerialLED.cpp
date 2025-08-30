#include <stdio.h>
#include <cstdint>
#include <array>
#include "pico/stdlib.h"
#include "pico/time.h"
// #include "pico/sleep.h"

#include "hardware/clocks.h" // set_sys_clock_khz
#include "./WS2812/include/WS2812.h"

#define PIN_WS2812_1 22     // GPIO 22 (29pin)
#define BUTTON_PIN_ENTER 28 // GP28 をプルアップ入力で使用（スイッチ）
#define BUTTON_PIN_SET 27   // GP27をキャラ変更で使用

#include "PatSignal.h" // パターン配列とカウント
#include "PatMario.h" // マリオのパターン配列とカウント
#include "PatZelda.h"	// ゼルダのパターン配列とカウント
#include "./WS2812/include/GammaCorrector.h"
#include "PatManager.h"
static volatile uint8_t timer_count = 0;

enum STATE {
	STATE_HIBER = 0,
	STATE_STOP = 1,
	STATE_START = 2,
	STATE_WALKING = 3,
	STATE_RUNNING = 4
};
STATE iState = STATE_HIBER; // 0が開始、1が停止状態、2が歩き、3が走り

// デバウンス付き：押下（プルアップなので Active-Low）を1回だけ検出
// 任意GPIOのボタンを処理できるよう、GPIOごとに状態を保持する
static bool button_pressed(uint gpio_pin)
{
	struct DebounceState {
		bool used = false;
		uint pin = 0;
		bool last_reading = true; // 直近の生読み取り
		bool debounced = true;    // デバウンス後の安定状態
		uint32_t last_change_ms = 0;
		bool armed = true;        // 押下1回につき1度だけtrueを返す
		bool initialized = false; // 初回初期化済みフラグ
	};

	// 同時に扱うGPIOの上限（必要に応じて拡張可）
	static DebounceState states[8];

	// スロット検索/確保
	DebounceState* st = nullptr;
	for (auto& s : states) {
		if (s.used && s.pin == gpio_pin) {
			st = &s;
			break;
		}
	}
	if (!st) {
		for (auto& s : states) {
			if (!s.used) {
				st = &s;
				break;
			}
		}
	}
	if (!st) {
		// スロット不足時は最初を使い回し（稀なケース）
		st = &states[0];
	}
	if (!st->used) {
		st->used = true;
		st->pin = gpio_pin;
		st->last_reading = gpio_get(gpio_pin);
		st->debounced = st->last_reading;
		st->last_change_ms = to_ms_since_boot(get_absolute_time());
		st->armed = true;
		st->initialized = true;
	}

	bool reading = gpio_get(gpio_pin);
	uint32_t now_ms = to_ms_since_boot(get_absolute_time());

	if (reading != st->last_reading) {
		st->last_change_ms = now_ms; // 変化を検出、デバウンス計測開始
		st->last_reading = reading;
	}

	// 30ms安定したら確定
	if ((now_ms - st->last_change_ms) > 30) {
		if (st->debounced != reading) {
			st->debounced = reading;
			if (st->debounced == false) {
				// 立下り＝押下確定（Active-Low）
				if (st->armed) {
					st->armed = false; // 次の解放まで無効化
					return true;
				}
			} else {
				// 解放で再武装
				st->armed = true;
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
		iState = STATE_STOP;
		return false; // タイマーは停止
	}
	return true;
}

void gpio_callback(uint gpio, uint32_t events)
{
	if (gpio == BUTTON_PIN_ENTER) {
		// 割り込み発生時の処理
		gpio_put(PICO_DEFAULT_LED_PIN, 0);
	} else if (gpio == BUTTON_PIN_SET) {
		// 割り込み発生時の処理
		gpio_put(PICO_DEFAULT_LED_PIN, 1);
	}
}

struct COLOR_RANGE {
	uint8_t min;
	uint8_t max;
};

class Patterns {
	public:
	const std::uint32_t* PatStopFlat;			/// 停止パターンは１つだけ
	const std::uint32_t* PatWalkFlat[4];		///　パターングループとして４つまで登録可能
	size_t PatWalkCount;						/// 各パターンの数。本当は、パターングループに含まれるパターンごとに違う可能性もあるが、ゲームという特性上ほぼ同じなので１つにする。
	COLOR_RANGE GreenRange;						/// 
	COLOR_RANGE RedRange;
	COLOR_RANGE BlueRange;
	float Gamma;
	uint8_t BrightnessPercent;
	uint8_t ContrastPercent;
	bool isColorReplace;
	bool isOverlay;
	uint16_t iWaitWalk;
	uint16_t iWaitRun;

	void setPatManager(PatManager &pmStay,  PatManager* pmRun) 
	{
		pmStay.init(PatStopFlat, 1, 16, 16);
		pmStay.setGreenRange(GreenRange.min, GreenRange.max);
		pmStay.setRedRange(RedRange.min, RedRange.max);
		pmStay.setBlueRange(BlueRange.min, BlueRange.max);
		if (Gamma > 0.0f) pmStay.setGamma(Gamma);
		if (BrightnessPercent !=0 || ContrastPercent != 0) pmStay.setBrightnessContrast(BrightnessPercent, ContrastPercent);

		pmRun[0].init(PatWalkFlat[0], PatWalkCount, 16, 16);
		pmRun[0].setGreenRange(GreenRange.min, GreenRange.max);
		pmRun[0].setRedRange(RedRange.min, RedRange.max);
		pmRun[0].setBlueRange(BlueRange.min, BlueRange.max);
		if (Gamma > 0.0f) pmRun[0].setGamma(Gamma);
		if (BrightnessPercent !=0 || ContrastPercent != 0) pmRun[0].setBrightnessContrast(BrightnessPercent, ContrastPercent);

	}

} CharInfo[] = {
	{LGMRed, {&LGMPat[0][0], NULL, NULL, NULL}, LGMPatCount, {0, 0}, {0, 0}, {0, 0}, 1.0f, 0, 0, true, true, iWaitLGMWalk, iWaitLGMRun},
	{MROStay, {&MRORun[0][0], NULL, NULL, NULL}, MROPatCount, {0, 16}, {8, 32}, {8, 48}, 1.0f, 0, 0, false, false, iWaitMarioWalk, iWaitMarioRun},
	{ZELDAStay, {&ZELDARight[0][0], &ZELDAFront[0][0], &ZELDALeft[0][0], &ZELDABack[0][0]}, ZELDARightCount, {0, 16}, {0, 16}, {0, 16}, 1.0f, 0, 0, false, false, iWaitZELDAWalk, iWaitZELDARun}};

PatManager pmRun[4];
PatManager pmStay;
int main()
{

	// WS2812 のタイミング定数が 125MHz 前提のため、起動直後に 125MHz に固定
	set_sys_clock_khz(125000, true);

	stdio_init_all();
	// 1枚 8x8 パネルを前提（必要に応じて枚数を変更）
	WS2812 led_matrix(PIN_WS2812_1, 16, 16);

	// ボタン(GP28)をプルアップ入力で初期化
	gpio_init(BUTTON_PIN_ENTER);
	gpio_pull_up(BUTTON_PIN_ENTER);
	gpio_set_dir(BUTTON_PIN_ENTER, GPIO_IN);

	gpio_init(BUTTON_PIN_SET);
	gpio_set_function(BUTTON_PIN_SET, GPIO_FUNC_SIO);
	gpio_pull_up(BUTTON_PIN_SET);
	gpio_set_dir(BUTTON_PIN_SET, GPIO_IN);

	led_matrix.Reset();
	led_matrix.Clear(0);
	led_matrix.ScanBuffer();
	int currPatNo = 0;
	int prevPatNo = 0;

	// 時刻開始（マイクロ秒単位）
	uint64_t start_us = time_us_64();
	repeating_timer_t rt;

	int iCharNo = 0;
	set_sys_clock_khz(125000, true);
	

	while (true) {

		while (true) {
			if (iState == STATE_HIBER) {
				// アクティブローのボタンなので、押下で起床するよう立下りエッジで割り込み
				gpio_set_irq_enabled_with_callback(BUTTON_PIN_ENTER, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
				gpio_set_irq_enabled_with_callback(BUTTON_PIN_SET,   GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
				led_matrix.Reset();
				led_matrix.Clear(0);
				led_matrix.ScanBuffer();
				__asm("  wfi");
				gpio_set_irq_enabled_with_callback(BUTTON_PIN_ENTER, GPIO_IRQ_EDGE_FALL, false, &gpio_callback);
				gpio_set_irq_enabled_with_callback(BUTTON_PIN_SET,   GPIO_IRQ_EDGE_FALL, false, &gpio_callback);

				iState = STATE_STOP;

			} else if (iState == STATE_STOP) {
				led_matrix.Reset();
				CharInfo[iCharNo].setPatManager(pmStay, pmRun);



				if (CharInfo[iCharNo].isColorReplace) {
					const std::uint32_t* buf = static_cast<const std::uint32_t*>(pmStay.getBufferPtr(0));
					led_matrix.DrawBuffer(buf, 16, 16, 0, 0, 0x000700, false); // パターンを描画
				} else {
					const std::uint32_t* buf = static_cast<const std::uint32_t*>(pmStay.getBufferPtr(0));
					led_matrix.DrawBuffer(buf, 16, 16, 0, 0, 0, false); // パターンを描画
				}
				led_matrix.ScanBuffer(true, false);

				iState = STATE_START;
			} else if (iState == STATE_START) {
				// アイドル監視: STATE_STARTに入ってからの無操作時間を計測
				static uint32_t idle_start_ms = 0;
				uint32_t now_ms_state = to_ms_since_boot(get_absolute_time());
				if (idle_start_ms == 0) idle_start_ms = now_ms_state;

				// ボタンが押されたら、１０秒間隔のタイマーを開始する。
				if (button_pressed(BUTTON_PIN_ENTER)) {
					// 時刻開始（マイクロ秒単位）
					// １０秒間隔タイマー
					if (!add_repeating_timer_ms(10000, one_shot_cb, NULL, &rt)) {
						printf("Failed to add timer\n");
						return 1;
					}
					idle_start_ms = 0; // 動作開始でアイドル計測はリセット
					iState = STATE_WALKING;
				} else if (button_pressed(BUTTON_PIN_SET)) {
					// キャラ変更ボタンが押された場合の処理
					iCharNo++;
					if (iCharNo >= (sizeof(CharInfo) / sizeof(CharInfo[0]))) {
						iCharNo = 0;
					}	
					idle_start_ms = now_ms_state; // 操作があったので起点を更新
					iState = STATE_STOP;
				} else {
					// 無操作が30秒続いたら休止へ
					if ((now_ms_state - idle_start_ms) >= 30000u) {
						idle_start_ms = 0;
						iState = STATE_HIBER;
					}
				}


			} else if (iState == STATE_WALKING || iState == STATE_RUNNING) {
				int iWaitMs;
				int iTransMs;
				if (timer_count < 5) {
					iWaitMs = CharInfo[iCharNo].iWaitWalk;
					iTransMs = iWaitMs / 4;
				} else {
					iWaitMs = CharInfo[iCharNo].iWaitRun;
					iTransMs = iWaitMs / 6;
				}
				// 一つ前のパターンを暗く表示
				led_matrix.Clear(0);

				led_matrix.Reset();
				if (CharInfo[iCharNo].isColorReplace) {
					const std::uint32_t* bufPrev = static_cast<const std::uint32_t*>(pmRun[0].getBufferPtr(prevPatNo)); // 明示的にconstへ
					const std::uint32_t* bufCurr = static_cast<const std::uint32_t*>(pmRun[0].getBufferPtr(currPatNo)); // 明示的にconstへ
					led_matrix.DrawBuffer(bufPrev, 16, 16, 0, 0, 0x030000, CharInfo[iCharNo].isOverlay);              // パターンを描画 (オーバーレイで短い時間を表示)
					led_matrix.DrawBuffer(bufCurr, 16, 16, 0, 0, 0x060000, CharInfo[iCharNo].isOverlay);             // パターンを描画 (オーバーレイで短い時間を表示)
				} else {
					const std::uint32_t* bufPrev = static_cast<const std::uint32_t*>(pmRun[0].getBufferPtr(prevPatNo)); // 明示的にconstへ
					const std::uint32_t* bufCurr = static_cast<const std::uint32_t*>(pmRun[0].getBufferPtr(currPatNo)); // 明示的にconstへ
					led_matrix.DrawBuffer(bufPrev, 16, 16, 0, 0, 0, CharInfo[iCharNo].isOverlay);                                      // パターンを描画 (オーバーレイで短い時間を表示)
					led_matrix.DrawBuffer(bufCurr, 16, 16, 0, 0, 0, CharInfo[iCharNo].isOverlay);                                      // パターンを描画 (オーバーレイで短い時間を表示)
				}
				led_matrix.ScanBuffer(true, false);
				sleep_ms(iTransMs);

				led_matrix.Reset();
				if (CharInfo[iCharNo].isColorReplace) {
					const std::uint32_t* buf = static_cast<const std::uint32_t*>(pmRun[0].getBufferPtr(currPatNo));    // 明示的にconstへ
					led_matrix.DrawBuffer(buf, 16, 16, 0, 0, 0x070000, CharInfo[iCharNo].isOverlay);                // パターンを描画
				} else {
					const std::uint32_t* buf = static_cast<const std::uint32_t*>(pmRun[0].getBufferPtr(currPatNo)); // 明示的にconstへ
					led_matrix.DrawBuffer(buf, 16, 16, 0, 0, 0, CharInfo[iCharNo].isOverlay);                                      // パターンを描画
				}
				led_matrix.ScanBuffer(true, false);

				// led_matrix.Keep();
				sleep_ms(iWaitMs);
				prevPatNo = currPatNo;             // 前のパターン番号を保存
				if (++currPatNo >= CharInfo[iCharNo].PatWalkCount) { // パターン番号設定
					currPatNo = 0;                 // パターン番号をリセット
				}
			}
		}
	}
}
