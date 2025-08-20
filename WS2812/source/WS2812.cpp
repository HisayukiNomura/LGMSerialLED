// PIOベースのWS2812送信
//
// モジュール概要:
// - PIOでWS2812(NeoPixel) の1線式プロトコルを生成し、VRAMからフレームを送出する。
// - PIOプログラムは 1bit=10サイクル設計（T1/T2/T3合算）。SMクロックは 8MHz(=800kHz*10) を目安に分周。
// - データはGRB順の24bit。CPU→PIOはTX FIFOにブロッキング書き込み。
//
// 期待される使い方の契約:
// - フレーム送出開始前に Reset() を呼び、ScanBuffer() で全画素を送る。送出後は Keep() でHigh維持（任意）。
// - VRAMは 0x00GGRRBB 形式で保持。SetPixel/Clear で更新し、Scan* 系で送出する。
// - 物理配線が千鳥（serpentine）の場合は ScanPanel の走査順を調整する（コメント参照）。
//
// 注意事項/制約:
// - 本実装はCPU供給（ポーリング）でFIFOを埋める。高解像度や高フレームレートではDMA化を検討。
// - PIO側のタイミング（命令数）を変更した場合、cycles_per_bit も一致させる必要がある。
// - Keep() はSMが有効であることを前提にHighを維持。SM無効化時はGPIO制御権が戻るため状態保持は保証しない。
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "WS2812.h"
#include "ws2812.pio.h" // PIOアセンブリをインクルード（.pio はビルドで .h に生成される想定)

/**
 * @brief WS2812送信用にPIOステートマシンを初期化します。
 *
 * - sideset ピンをデータピンに割り当て
 * - 24bit MSB-first で autopull 有効化
 * - TX FIFO を結合（送信専用）
 * - WS2812の1bitあたりのサイクル数(T1+T2+T3=10)から分周を算出
 *
 * @param pio          使用する PIO (pio0 / pio1)
 * @param sm           使用するステートマシン番号
 * @param offset       プログラムロードオフセット（pico_generate_pio_header 生成ヘッダのオフセット）
 * @param pin          データ出力GPIO番号
 * @param bit_freq_hz  ビットレート(例: 800000.0f)
 */
static inline void ws2812_program_init(PIO pio, uint sm, uint offset, uint pin, float bit_freq_hz) {
	// ステートマシン構成:
	// - sideset: データピンをPIO命令のサイドセットで駆動（タイミングを命令境界で制御）
	// - set_pins: PIOプログラム内の idle/out0/out1 ループで明示的にレベルを保持するためにも割当
	// - out_shift: 24bit自動プル（MSBファースト）で、1ピクセル=1回のプルに整合
	// - FIFO結合: TX側のみ使用し、深いFIFOでCPU側の書き込み負荷を緩和
	pio_sm_config c = ws2812_program_get_default_config(offset);
	sm_config_set_sideset_pins(&c, pin);
	// 'set' 命令経由でもデータピンを直接操作できるようマッピング
	sm_config_set_set_pins(&c, pin, 1);
	sm_config_set_out_shift(&c, false, true, 24); // MSB first, autopull 24-bit
	sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
	pio_gpio_init(pio, pin);
	pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

	// タイミング設定:
	// - PIOプログラム側の1bitシーケンスは合計10サイクル設計（例: T1=3, T2=3, T3=4 など）
	// - 目標ビットレート bit_freq_hz（通常 800kHz）から SMクロック=bit*10 を算出
	// - clk_sys を分周して SMクロックを生成
	const float cycles_per_bit = 10.0f;
	float sm_clk = bit_freq_hz * cycles_per_bit; // 800kHz * 10 = 8MHz
	float div = (float)clock_get_hz(clk_sys) / sm_clk;
	sm_config_set_clkdiv(&c, div);

	pio_sm_init(pio, sm, offset, &c);
	pio_sm_set_enabled(pio, sm, true);
}

/**
 * @brief WS2812 ドライバを生成します。
 *
 * PIOへ ws2812 プログラムをロードし、指定ピンで 800kHz 動作に初期化します。
 * また、パネル配置から VRAM サイズを計算し領域を確保します。
 *
 * @param pin             データ出力GPIO
 * @param a_xSize         1枚のパネルの幅（ピクセル数）
 * @param a_ySize         1枚のパネルの高さ（ピクセル数）
 * @param a_xPanelCount   パネルの水平方向枚数
 * @param a_yPanelCount   パネルの垂直方向枚数
 */
WS2812::WS2812(uint8_t pin,uint8_t a_xSize, uint8_t a_ySize , uint8_t a_xPanelCount,uint8_t a_yPanelCount) 
	: m_pin(pin) , xSize(a_xSize), ySize(a_ySize), xPanelCount(a_xPanelCount), yPanelCount(a_yPanelCount)
{
	// PIOプログラムのロードとSM確保:
	// - pio0 を使用。空きSMを強制確保（true指定: 見つからない場合はpanic）。
	// - フォールバックでpio1を使う改善は将来の拡張点。
	m_pio = pio0;
	m_offset = pio_add_program(m_pio, &ws2812_program);
	m_sm = pio_claim_unused_sm(m_pio, true);
	// 送信タイミング初期化: 800kHz（T=1.25us）に分周設定
	ws2812_program_init(m_pio, m_sm, m_offset, m_pin, 800000.0f);

	// VRAM割り当て:
	// - 総画素数 = (xSize*xPanelCount) * (ySize*yPanelCount)
	// - 1画素=24bit(実メモリは32bit)のGRB。0x00GGRRBB 形式で保持。
	// - メモリ使用量は画素数*4バイト。高解像度ではヒープを圧迫する点に注意。
	xVRam = xSize * xPanelCount; // VRAMのXサイズ
	yVRam = ySize * yPanelCount; // VRAMのYサイズ
	pVRam = new uint32_t[xVRam * yVRam];
	for (uint32_t i = 0; i < xVRam * yVRam; i++) pVRam[i] = 0;


}



/**
 * @brief WS2812 のリセットラッチ時間を満たす Low パルスを出力します。
 *
 * ステートマシンを再起動して FIFO をクリアし、PIOプログラムの out0 ループへジャンプして
 * 50us 以上の Low を維持した後、送信ループの先頭へ復帰します。
 *
 * @note フレーム送信前のラッチに使用します。
 */
void WS2812::Reset() {
	// 送信前ラッチ手順:
	// 1) SMを停止→FIFOクリア→リスタートで内部状態を既知化
	// 2) out0ループへJMPし、ラインをLowで維持（>50us）
	// 3) プログラム先頭へ戻して通常送信ループに復帰
	//
	// WS2812の仕様上、リセットラッチはおおむね 50µs 以上が必要。本実装は80µsを確保。
	sleep_us(500); // 前フレーム終端からの安全マージン
	pio_sm_set_enabled(m_pio, m_sm, false);
	pio_sm_clear_fifos(m_pio, m_sm);
	pio_sm_restart(m_pio, m_sm);
	pio_sm_set_enabled(m_pio, m_sm, true);
	// out0 ループへジャンプしてリセットパルス（>50us）
	pio_sm_exec(m_pio, m_sm, pio_encode_jmp(m_offset + ws2812_offset_out0));
	sleep_us(80);

	pio_sm_restart(m_pio, m_sm);
	pio_sm_exec(m_pio, m_sm, pio_encode_jmp(m_offset));
	pio_sm_set_enabled(m_pio, m_sm, true);

}
/**
 * @brief アイドル時の High 出力を維持します。
 *
 * PIOプログラムの idle ループへジャンプして、ラインを High に保ちます。
 */
void WS2812::Keep()
{
	// アイドル維持:
	// - PIOプログラムの idle ラベルへJMP。サイドセットでHighを出し続ける無限ループ。
	// - SMを無効化するとPIO制御が解けるため、状態保持は保証されない点に注意。
	pio_sm_exec(m_pio, m_sm, pio_encode_jmp(m_offset + ws2812_offset_idle));
}
/**
 * @brief 1ピクセル分の GRB データを即時送信します（ブロッキング）。
 * @param r 赤(0-255)
 * @param g 緑(0-255)
 * @param b 青(0-255)
 * @note VRAMを使わず直接送信します。フレーム送信には ScanPanel/ScanBuffer を推奨。
 */
void WS2812::setColorDirect(uint8_t r, uint8_t g, uint8_t b) {
	// 即時送信（ブロッキング）:
	// - PIOのシフトはMSBファースト/24bit自動プル。
	// - 32bit FIFOの上位24bitに詰めるため <<8 して送る。
	// - FIFOが満杯の場合は空くまで待つ。
	uint32_t grb = ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
	pio_sm_put_blocking(m_pio, m_sm, grb << 8); // 24bitを左寄せ（PIO側はautopull 24bit）
}
/**
 * @brief 24bit GRB 値を即時送信します（ブロッキング）。
 * @param c 0x00GGRRBB 形式の24bitカラー（GRB順）
 */
void WS2812::setColorDirect(uint32_t c)
{
	// 入力形式: 0x00GGRRBB（上位8bit未使用）。左へ8bitシフトして上位24bitに配置。
	pio_sm_put_blocking(m_pio, m_sm, c << 8); // 24bitを左寄せ（PIO側はautopull 24bit）
}
/**
 * @brief テスト用にランダム色を1ピクセル送信します。
 */
void WS2812::fillRandomColorDirect() {
	static uint32_t s = 0x12345678u;
	// 疑似乱数（xorshift）で擬似的に色変化。下のrand()は微小値に抑え輝度過多を回避。
	s ^= s << 13; s ^= s >> 17; s ^= s << 5;

	uint8_t r = (uint8_t)(rand() % 2+ 1);
	uint8_t g = (uint8_t)(rand() % 2 + 1);
	uint8_t b = (uint8_t)(rand() % 2 + 1);
//	uint8_t r = (uint8_t)(0);
//	uint8_t g = (uint8_t)(0);
//	uint8_t b = (uint8_t)(0);
	setColorDirect(r, g, b);
}

/**
 * @brief VRAM全体を指定色で塗りつぶします。
 * @param rgb 24bit GRB カラー
 */
void WS2812::Clear(uint32_t rgb)
{
	// VRAM全体を1色で初期化。描画のベース色や消去に使用。
	for (uint32_t i = 0; i < xVRam * yVRam; ++i) pVRam[i] = rgb;
}
/**
 * @brief 指定座標のピクセルを設定します（VRAMのみ）。
 * @param x X座標（0..xVRam-1）
 * @param y Y座標（0..yVRam-1）
 * @param rgb 24bit GRB カラー
 */
void WS2812::SetPixel(uint16_t x, uint16_t y, uint32_t rgb)
{
	// 範囲内なら1画素だけ更新（VRAM）。送信は行わない。
	if (x < xVRam && y < yVRam) pVRam[y * xVRam + x] = rgb;
}

/**
 * @brief 1パネル分のデータを送信します（VRAM→PIO）。
 * @param posX パネル左上のVRAM X座標
 * @param posY パネル左上のVRAM Y座標
 */
void WS2812::ScanPanel(uint8_t posX, uint8_t posY, bool serpentine, bool leftToRight)
{
	// 1パネル走査:
	// - 左上(posX,posY)原点から行優先でVRAMを読み、24bitピクセルを連続送信。
	// - 物理が千鳥配線（偶数行/奇数行で左右反転）の場合は、
	//   yが奇数のとき x の走査方向を反転する。
	for (uint8_t y = 0; y < ySize; ++y) {
		// 偶数行の基準方向: leftToRight
		bool baseL2R = leftToRight;
		// 千鳥配線なら奇数行で左右反転
		bool l2r = serpentine ? ((y & 1u) ? !baseL2R : baseL2R) : baseL2R;
		if (l2r) {
			for (uint8_t x = 0; x < xSize; ++x) {
				uint32_t color = pVRam[(posY + y) * xVRam + (posX + x)];
				setColorDirect(color);
			}
		} else {
			for (int8_t x = (int8_t)xSize - 1; x >= 0; --x) {
				uint32_t color = pVRam[(posY + y) * xVRam + (posX + (uint8_t)x)];
				setColorDirect(color);
			}
		}
	}
}

/**
 * @brief 指定したパネルの外枠を塗りつぶします（VRAMのみ）。
 * @param panelX パネルXインデックス（0..xPanelCount-1）
 * @param panelY パネルYインデックス（0..yPanelCount-1）
 * @param rgb    24bit GRB カラー
 */
void WS2812::DrawPanelBorder(uint8_t panelX, uint8_t panelY, uint32_t rgb)
{
	if (panelX >= xPanelCount || panelY >= yPanelCount) return;
	uint16_t baseX = panelX * xSize;
	uint16_t baseY = panelY * ySize;

	// 外枠描画: 上下辺（幅xSize）を走査
	for (uint8_t x = 0; x < xSize; ++x) {
		pVRam[(baseY + 0) * xVRam + (baseX + x)] = rgb;                 // 上
		pVRam[(baseY + (ySize - 1)) * xVRam + (baseX + x)] = rgb;       // 下
	}
	// 外枠描画: 左右辺（高さySize）を走査
	for (uint8_t y = 0; y < ySize; ++y) {
		pVRam[(baseY + y) * xVRam + (baseX + 0)] = rgb;                 // 左
		pVRam[(baseY + y) * xVRam + (baseX + (xSize - 1))] = rgb;       // 右
	}
}

/**
 * @brief 全パネルの外枠をランダム色で塗ります（VRAMのみ）。
 * @note 現実装は行（panelY）ごとに同一色になります。
 */
void WS2812::DrawRandomBorders()
{
	// 行（panelY）単位で色を変える簡易デモ。
	// - 実運用では輝度スケーリングやHSV色相回転などの演出関数に差し替え可能。
	static uint32_t s = 0x89abcdefu;
	auto rnd = [&]() {
		s ^= s << 13; s ^= s >> 17; s ^= s << 5; return s; };

	for (uint8_t py = 0; py < yPanelCount; ++py) {
		uint8_t r = (uint8_t)(rand() % 2 + 1);
		uint8_t g = (uint8_t)(rand() % 2 + 1);
		uint8_t b = (uint8_t)(rand() % 2 + 1);

		for (uint8_t px = 0; px < xPanelCount; ++px) {
			uint32_t grb = ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
			DrawPanelBorder(px, py, grb);
		}
	}
}

void WS2812::DrawBuffer(const uint32_t pattern[], uint8_t width, uint8_t height, uint8_t X, uint8_t y,uint32_t colorReplace,bool isOverlay)
{
	bool bisReplace = colorReplace != 0x0;

	for (uint8_t py = 0; py < height; ++py) {
		for (uint8_t px = 0; px < width; ++px) {
			uint32_t color = pattern[py * width + px];
			if (bisReplace) {
				if (color != 0x000000) {
					SetPixel(X + px, y + py, colorReplace/* 0x0F0001*/);
				} else {
					if (isOverlay == false) {
						SetPixel(X + px, y + py, 0);
					}
				}
			} else {
				if (isOverlay== false) {
					SetPixel(X + px, y + py, color);
				} else {
					if (color != 0x000000) {
						SetPixel(X + px, y + py, color);
					} else {
					}
				}
			}
		}
	}
}

/**
 * @brief 全パネルを走査してフレームを送信します（VRAM→PIO）。
 *
 * 左上から右下へパネル順に送信します。
 *
 */
void WS2812::ScanBuffer(bool serpentine, bool leftToRight)
{
	// 全パネル走査（行優先）:
	// - Reset() 直後に呼ぶ想定。安全余裕の待ち時間を確保してから送信開始。
	// - 高速化が必要なら: パネル/行単位でDMAを使ってFIFOへ連搬する。
	sleep_us(100); // 直前のリセットからの安全待ち（環境に合わせて最適化可）
	for (int y = 0; y < yPanelCount; y++) {
		for (int x = 0; x < xPanelCount; x++) {
			ScanPanel(x * xSize, y * ySize, serpentine, leftToRight); // パネルごとにデータを送信
		}
	}
}


