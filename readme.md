# 小緑人とは / What is Little Green Man / 什麼是小綠人

手っ取り早くソースコードが見たい、という人は[ここをクリック](#ソースコード)

## 日本語

台湾が好きで時々遊びに行きます。食べ物がおいしく、観光地もあります。台北を含む各地に電気街もあります。

## English

I love Taiwan and visit there from time to time. The food is delicious, there are tourist attractions, and there are electronics districts in various places including Taipei.

## 繁體中文

我喜歡台灣，時常去旅遊。食物美味，有很多觀光景點，台北等各地都有電子街。

## 日本語

台北の電気街は光華商場というビルを中心とした一角です。光華商場には1Fに名だたるPCメーカーが軒を連ね、２Fは大手ショップやマザーボードメーカと、上に行くにしたがって、怪しいお店が増えていき、５Fくらいからワケがワカラナイ、８０年代の秋葉原のようになってきます。

お隣の光華國際電子廣場には、B1、B2に電子工作好きの人向けのショップが集まっています。大手のお店は、秋月電子とマルツ（日本橋なら共立？）を合わせたようなお店や、アイテンドーの本店のようなお店もたくさんありますし、小さいお店は国際ラジオか日米か、という感じです。

<img width=50% src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/7aa14eff-a96d-4af7-8941-3babd25efe0c.jpeg">

さて、本題です。台湾に行った人は誰でも一度は必ず、歩行者用信号のアニメーションを目にします。
前々から、これがなかなか可愛らしいと思っていたのですが、私だけではなかったようで「小緑人（シャオリューレン）」と呼ばれて台湾の名物になっています。現地には記念碑もありますし、キーホルダーやブローチ、マグネットなどにもなっています。

![Walkingman.gif](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/6fa3928e-1c6e-43ec-9296-2c478be85982.gif)

やはり、これは一家に一台、絶対に必要なものだと言えるでしょう。

しかし、悲しいかな売っているものでもなく、また、さすがに信号機を手元に置くわけにもいきません。となると、自分好みの小緑人を作るしかありません。

## English

The electronics district in Taipei is centered around a building called Guanghua Market. On the first floor of Guanghua Market, famous PC manufacturers line up, the second floor has major shops and motherboard manufacturers, and as you go higher, more suspicious shops appear, and from around the 5th floor, it becomes incomprehensible, like Akihabara in the 1980s.

In the neighboring Guanghua International Electronics Plaza, shops for electronics hobbyists are gathered on B1 and B2. Major shops are like a combination of Akizuki Electronics and Marutu (or Kyoritsu in Nihonbashi), and there are many shops like the main store of iTendo, while smaller shops feel like International Radio or Nichimei.

Now, let's get to the main topic. Anyone who has been to Taiwan has definitely seen the pedestrian traffic light animation at least once. I've always thought it was quite cute, and it seems I wasn't alone, as it's called "Little Green Man (Xiǎo Lǜ Rén)" and has become a specialty of Taiwan. There are even monuments to it locally, and it's made into keychains, brooches, magnets, and more.

This is definitely something that every household absolutely needs.

However, sadly, it's not something that's sold, and you can't exactly put a traffic light at home. So, the only option is to create your own Little Green Man.

## 繁體中文

台北的電子街是以光華商場這棟大樓為中心的一個區域。光華商場一樓有知名PC廠商並排，二樓是大型商店和主機板廠商，往上層樓愈來愈多可疑的商店，從五樓左右開始變得莫名其妙，就像80年代的秋葉原一樣。

鄰近的光華國際電子廣場，地下一樓和地下二樓聚集了針對電子製作愛好者的商店。大型商店像是秋月電子和マルツ（在日本橋的話是共立？）的結合體，或像アイテンドー本店這樣的商店也很多，小商店則給人國際ラジオ或日米的感覺。

現在進入正題。去過台灣的人都一定至少看過一次行人號誌的動畫。我一直覺得這很可愛，而且看來不只我這麼想，它被稱為「小綠人（Xiǎo Lǜ Rén）」，成為台灣的名物。當地還有紀念碑，也被做成鑰匙圈、胸針、磁鐵等等。

這確實是每個家庭絕對需要的東西。

然而，遺憾的是這不是有在販賣的東西，而且也不可能把交通號誌放在手邊。所以只能製作自己喜歡的小綠人了。

# 実際の小緑人 / Actual Little Green Man / 實際的小綠人

## 日本語

現地で観察したところ、よく見かける小緑人には16x16マトリックスLEDのもの、独立したLEDを必要な場所に配置してあるものがあります。（それ以外には、バイク用のもの、円形のもの、より高い解像度のものもあります）

独立したLEDのものは、LEDの総数が4～50個ですがLEDの配置を自分で考える必要があること、結線を自分で考える必要があるのでかなり難しそうです。パターンの修正はハードウェア変更が必要になります。

こう考えると、マトリックスLEDで実現するのが現実的でしょう。

## English

From observation on-site, the commonly seen Little Green Man displays include 16x16 matrix LED types and those with individual LEDs placed at necessary positions. (There are also motorcycle versions, circular ones, and higher resolution ones)

For individual LED types, while the total number of LEDs is 4-50, you need to design the LED placement and wiring yourself, which seems quite difficult. Pattern modifications would require hardware changes.

Considering this, implementing with matrix LEDs would be more practical.

## 繁體中文

根據現場觀察，常見的小綠人有16x16矩陣LED的，以及將獨立LED配置在必要位置的類型。（除此之外還有機車用的、圓形的、更高解析度的等等）

獨立LED類型雖然LED總數為4～50個，但需要自己考慮LED的配置，也需要自己考慮接線，看起來相當困難。圖案的修改需要硬體變更。

這樣考慮的話，用矩陣LED來實現會比較實際。

# 走れ！小緑人 / Run! Little Green Man / 跑吧！小綠人

## 日本語

これが作成された小緑人です。
<iframe width="320" height="320" src="https://www.youtube.com/embed/Dc_bQSexsT8" title="小緑人" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>

起動時は待機状態（節電状態）で始まります。STARTボタンを押すと停止状態（歩行者信号の「止まれ」）になります。再度STARTボタンを押すと、小緑人が歩き出します。５０秒間は歩き、残り１０秒間は走ります。
停止状態が３０秒続くと、待機状態に戻ります。

せっかくのカラーLEDなので、ほかのキャラクターも歩かせるようにしてみました。

<iframe width="320" height="320" src="https://www.youtube.com/embed/Q-JZ319Bm3c" title="小緑人２" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>

待機状態（歩行者信号の「とまれ」）で、スイッチを押すと表示されるキャラクターが切り替わります。

ビデオ映像にすると、全体的に白っぽくなってしまいなんだかわかりにくなってしまいますが、有名なヒゲのおじさん、緑の服のエルフ、プププランドの若者、伝説の勇者です。

WS2812は、LED自体が大きく、ドットが密に配置されていません。そのため視認性はイマイチです。
光拡散シートなどを使って、LEDの隙間が見えにくくする必要がありそうです。ただ、そうすると小緑人「ぽさ」が失われてしまうのが悩みどころです。

## English

This is the created Little Green Man.

At startup, it begins in standby mode (power saving state). Pressing the START button changes it to stop mode (pedestrian signal "stop"). Pressing the START button again makes the Little Green Man start walking. It walks for 50 seconds, then runs for the remaining 10 seconds.
When the stop state continues for 30 seconds, it returns to standby mode.

Since we have color LEDs, I tried making other characters walk as well.

In standby mode (pedestrian signal "stop"), pressing the switch changes the displayed character.

While video footage makes everything look whitish and somewhat difficult to see, these are the famous mustached man, green-clothed elf, young person from Dream Land, and legendary hero.

WS2812 LEDs are large and dots are not densely arranged, so visibility is somewhat poor.
It seems necessary to use light diffusion sheets to make the gaps between LEDs less visible. However, this would reduce the Little Green Man "feel", which is a dilemma.

## 繁體中文

這是製作的小綠人。

啟動時以待機狀態（節電狀態）開始。按下START按鈕會變成停止狀態（行人號誌的「停止」）。再次按下START按鈕，小綠人就會開始走路。走50秒，剩下的10秒會跑步。
停止狀態持續30秒會回到待機狀態。

既然有彩色LED，我試著讓其他角色也走路。

在待機狀態（行人號誌的「停止」）下，按下開關會切換顯示的角色。

拍成影片的話，整體會變得偏白，有點難以看清，但分別是有名的鬍子大叔、綠衣精靈、夢幻島的年輕人、傳說勇者。

WS2812的LED本身很大，點陣並不密集。因此視認性不太好。
看來需要使用光擴散片等讓LED之間的縫隙不容易看見。不過這樣做的話會失去小綠人的「感覺」，這是個煩惱的地方。


# 工夫した点

小緑人は７パターンの繰り返しで動作しますが、これをそのまま表示すると絵がパカパカと変わってしまい、今一つな印象でした。そこで、暗くした前のパターンと今回のパターンを重ねたようなフレームを挿入し、滑らかに動くように見せています。
実際の小緑人をビデオに撮ったものでは、LEDの消え方は緩やかになっています。これが、単に目やビデオの錯覚なのかはわかりませんが、これを実装した形になっています。



# 制作のメモ


## マトリックスLEDの検討

まず、マトリックスLEDの部品を選定します。マトリックスLEDには、制御方法にいくつか種類があるようです。

- 直接制御
- HUB75
- WS2812

### 直接制御
格子状に結線されたLEDのマトリックスを、独立して制御します。

<img width = 50% src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/00e81c2a-d271-4d87-9483-3314b9953736.png">

そのままだと、16x16のLEDの場合GPIOが32本必要です。Raspberry pi PICOには30本のGPIOがありますが、数としては不足します。そのため、デコーダーなどを使う必要があるでしょう。

例えば、アノード側（COL側）は74xx4514、カソード側（ROW側）に74xx154を使うなどが使用できるかもしれません。74xx154/4514は、４ビット（0x0～0xF)を受け取り、16ビット側にそのビットを出力します。たとえば、74xx154では、0b0011 (0x3) を入力すると、出力側は A0から1101111111111111など、３ビット目だけがオフになります。（Active Low）　74xx4514はその逆で、出力側の３ビット目だけがオンになります。
マトリックスLEDでは、例えば列側に 0b0100、行側に0b1001 を出力すると、（4,9）の場所のLEDが点灯します。
わかりやすい方法ですが、TTLの入手方法に難ありです。手はんだが容易なDIP版はもう製造中止でDigiKey、Mouserどちらにも在庫がありません。

（なお、台湾の光華商場のビルの隣、光華國際電子廣場ではまだ普通に売っていました。買ってこなかったのは失敗です）


### HUB75

街角で見かける、大型のLEDディスプレイなどで使われているプロトコルです。情報も多く、使うポートは１６本です。

|ピンの機能|数|ピン名|
|---|---|---|
|Addr|4|A1～A4|
|LED|6|R1,G1,B1,R2,G2,B2|
|Utils|3|CLK,LAT,OE|
|その他|3|GND|

点灯方式はダイナミック点灯で、４本のアドレスピンに0～15を出力し、R1G1B1とR2G2B2で２行分出力するという方法です。

情報は多いのですが、実際に動かすのには大変そうです。

HUB75では、輝度指定がありません。調べると、輝度の指定は画面の点灯時間で制御する必要がある(いわゆるPWM）ようです。
つまり、HUB75ではLEDはダイナミック点灯で同時に点灯するのはROW一つだけ、さらに中間色を表現するにはPWMがひつっ用ということになります。そのため、中間色を出すには、画面のリフレッシュを高速で行い、点灯時間を制御する必要があります。<br>パネル単位、行単位で同じ輝度を使うにはさほど難しくはなさそうですが、ドット単位で輝度を変えるとなるとかなり難しい気がします。

もう少し深堀りしないときちんとしたものにはならなそうです。

HUB75では、LEDは普通の三色LEDを使います。そのため、パネル自体が安価になりそうです。ただ、調べた範囲では、個人が使う１枚２枚の数量では後述のWS2812に比べて特段安いということはありませんでした。

### WS2812

テープLEDなどで使われているもので、信号線は１本です。電源、GNDと合わせても3線でコントロールされます。

LED自体がインテリジェントなLEDで、データを受け取るとリセットされるまでその色で光り続けます。光り続けている間に、次の色指定が来るとそのまま次のLEDにそれを伝えます。

回覧板のようにデータが次のLEDに渡されることで、マトリックスLEDが表示されます。信号は、24ビットカラーで、輝度も変更できます。

信号は１本で24bitなので、16x16なら256個分、6Kbitを送信する必要があります。そのため信号は非常にシビアになっているようです。
たとえば、1ビットの送信では1.25μs、許容される誤差は±0.15usです。０と１は、オン⇒オフの切り替えで行いますが、0.2μsの違いで1と0が切り替わることになります。delay_msなどで適当に待つようなコードでは無理そうです。


## WS2812

結局、WS2812を使用することにしました。WSS2812のマトリクスLEDは、入手性が良く、価格も１枚２枚というレベルであればHUB75と変わりません。

[秋月電子　WS2812B搭載フルカラーLEDディスプレイ 16×16](https://akizukidenshi.com/catalog/g/g116382/) …　￥3,800
[ピカリ館](https://www.akiba-led.jp/product/960) …　￥9,800
[Aliexpress](https://ja.aliexpress.com/w/wholesale-ws2812-matrix-led.html?spm=a2g0o.tesla.search.0) …　￥2,000 ～ ￥4,000

購入時は、16x16のものが必要です。

### 配線図

結線図としては次のようになります。回路図を書くほどではありませんので回路図は省略です。

<img width=80% src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/58c1b12c-6deb-4685-b6a5-414b3c3168b0.png">

WS2812LEDには、３種類の線が出ています。
|名称|色|用途|
|---|---|---|
|DIN|白緑赤|信号の入力<br>最初のLEDマトリックスパネルでは、ここにマイコンなどのコントローラーを接続する|
|5V|赤黒|電源|
|DOUT|白緑赤|信号の出力<br>複数のLEDマトリックスを使用する場合、上流のDOUTに、下流のDINを接続する|

WS2812は、カスケード接続することができます。DINには、上流のパネルのDOUTを接続します。

WS2812はそれぞれのLEDが、CPUを持っています。データを受け取ると、そのデータに基づいて光りますが、もしすでに光っている場合、データをそのまま隣のLEDに渡します。パネルの末尾に来てもこの動作は変わらず、DOUTにそのまま次のデータを引き渡します。このデータを２枚目以降のパネルが受け取り、発光を続けます。

バケツリレーのように、データが次のLEDに引き渡されることになります。


### 制御プロトコル

WS2812は電源（+5VとGND)を除くと１ピンで制御します。

この１ピンに流す信号は非常にシンプルですが、タイミングがシビアです。基本的に１ビットを、1.25μs±600nsで送る必要があります。

![1.png](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/43707c6f-5778-41aa-b2f6-7848c54c6659.png)

許される誤差は、信号全体で±600ns、LOW/HIGHそれぞれの部分で±150nsです。

|送信データ|HIGHの時間|LOWの時間||
|---|---|---|---|
|RESET|なし|50μS以上||
|０|0.4μS|0.85μS|0.25～0.55μS|
|１|0.８|0.45μS||

許される誤差は、信号全体で±600ns、LOW/HIGHそれぞれの部分で±150nsです。

### 表示するフレームレート
まず、きちんとアニメーションできるスピードになるかを調べます。

LEDは24ビットフルカラーで点灯させるため、LEDを一つ制御するために、データを２４回送信する必要があります。
すると、1つのLEDを任意の色で点灯させるには(0.4μS + 0.85μS) x 24 = 30μS、16x16個のマトリックスLEDの場合、30μS x 256 = 7680μS また、最初にリセットを送るため、50μ秒が必要です。　7680 μS＋50μS ＝　7730μ秒となります。これは7.73msです。

１画面の更新に7.73msかかるので、秒間の更新数は129フレーム/秒になります。

この速度はアニメーションとして十分な速度です。

## Raspberry PI PICO側の構造

### 制御プロトコルの送信方法

WS2812の信号がタイミングとしてシビアなので、これを守ることができるかを調べます。
120MHzで動作しているのであれば、１クロックは 1/120,000,000 秒なので、8.33nS　となります。

一つの信号が1.25μ秒なので約１５０クロック。RP2040は簡単な命令だと１クロック１命令なので、可能は可能ですが、各ビットの送信の間で行われる処理（次のビットを準備する）などを、かなりシビアなタイミングで実行する必要がありそうで、これにはかなり神経質なプログラム作成が必要になりそうです。（命令数を数えて、NOPなどで調整する）

これはかなり避けたい事態ですが、Raspberry pi PICOには、PIOと呼ばれる便利機能があります。PIOは、本体のCPUとは別の、独立したステートマシンで動作します。これを使えば、メインプログラム側のタイミングに依存せず、細かい制御が実行できそうです。今回は、このPIO機能を使ってみることにしました。

FIFOで受け取ったデータを、逐次LEDに送出するというPIOプログラムを作成し、登録しておけばメインプログラム側は、FIFOへの書き込みだけでタイミングを気にする必要がなくなります。

## 表示方法

表示するためのデータは、よくある方法としてVRAMのように、オフスクリーンでメモリを用意し、そのメモリに値を書き込む、一定周期（または必要に応じて）そのVRAMのデータをWS2812に転送する方法としました。

メモリは、各ドットごとに32ビットのワードマップとし、２４ビットで色を表現します。最上位の残り８ビットは使用しません。

前述のように、１画面すべてを書き換えても7.73msなので、この辺は特に気にする必要はなさそうです。Raspberry PI は、マルチCPUなので、画面更新を別CPUで非同期に回すという手もあるでしょう。

これをクラスとしてまとめておけば、汎用的に使用できるかもしれません。

### ソースコード
ソースコードは[GitHub](https://github.com/HisayukiNomura/LGMSerialLED)にて公開しています。

---

# WS2812用のライブラリ / WS2812 Library / WS2812函式庫

本プログラムで使用されている、WS2812に任意の画像を表示するためのクラス。

複数のパネル用のコードも作ってありますが、パネルを一枚しか買っていないので試していない。

## ファイル構成 / File Structure / 檔案結構

| ファイル名 / File Name | 説明 / Description / 說明 |
|---|---|
| WS2812.cpp / WS2812.h | WS2812クラスのクラス定義と本体 / WS2812 class definition and implementation / WS2812類別定義和實作 |
| ws2812.pio | PIOファイル / PIO file / PIO檔案 |

> **注意 / Note / 注意**: CMakeLists.txtの`target_link_libraries`に`hardware_pio`の定義が必要です。  
> `hardware_pio` must be defined in `target_link_libraries` in CMakeLists.txt.  
> 需要在CMakeLists.txt的`target_link_libraries`中定義`hardware_pio`。

## 基本仕様 / Basic Specifications / 基本規格

| 項目 / Item / 項目 | 値 / Value / 值 | 説明 / Description / 說明 |
|---|---|---|
| 制御プロトコル / Control Protocol / 控制協定 | WS2812 (1-wire) | 1線式RGB LEDプロトコル / Single-wire RGB LED protocol / 單線RGB LED協定 |
| 色深度 / Color Depth / 色彩深度 | 24bit | RGB各8bit / 8 bits per RGB channel / RGB各8位元 |
| フレームレート / Frame Rate / 畫面更新率 | ~129 FPS | 16x16パネル時 / For 16x16 panel / 16x16面板時 |
| VRAM形式 / VRAM Format / VRAM格式 | 0x00GGRRBB | GRB順、上位8bit未使用 / GRB order, upper 8 bits unused / GRB順序，高8位元未使用 |

## 実装上の注意点 / Implementation Notes / 實作注意事項

### LEDマトリックスの配列と開始点 / LED Matrix Layout and Starting Point / LED矩陣配列和起始點

LEDマトリックスには複数の配線方式があります：  
LED matrices have multiple wiring methods:  
LED矩陣有多種配線方式：

#### 配線方式の種類 / Wiring Types / 配線類型

| 配線方式 / Wiring Type / 配線類型 | 説明 / Description / 說明 | 設定 / Setting / 設定 |
|---|---|---|
| 通常配線 / Normal | 全ての行が同じ方向 / All rows in same direction / 所有行方向相同 | `serpentine = false` |
| 千鳥配線 / Serpentine | 奇数行が反転 / Odd rows reversed / 奇數行反轉 | `serpentine = true` |

#### 開始点の違い / Starting Point Variations / 起始點變化

| 開始点 / Starting Point / 起始點 | 設定 / Setting / 設定 |
|---|---|
| 左上から右へ / Top-left to right / 左上向右 | `leftToRight = true` |
| 右上から左へ / Top-right to left / 右上向左 | `leftToRight = false` |

### VRAM座標系 / VRAM Coordinate System / VRAM座標系統

- VRAMは常に左上(0,0)から開始 / VRAM always starts from top-left (0,0) / VRAM總是從左上角(0,0)開始
- X軸は右方向、Y軸は下方向 / X-axis rightward, Y-axis downward / X軸向右，Y軸向下  
- 実際のLEDパネルとの変換は`ScanBuffer`/`ScanPanel`で処理 / Conversion to actual LED panel handled by `ScanBuffer`/`ScanPanel` / 與實際LED面板的轉換由`ScanBuffer`/`ScanPanel`處理

### パフォーマンス考慮事項 / Performance Considerations / 效能考慮事項

| 項目 / Item / 項目 | 値 / Value / 值 | 備考 / Notes / 備註 |
|---|---|---|
| 1フレーム更新時間 / Frame update time / 畫面更新時間 | ~7.73ms | 16x16パネル時 / For 16x16 panel / 16x16面板時 |
| 最大フレームレート / Max frame rate / 最大畫面更新率 | ~129 FPS | 理論値 / Theoretical / 理論值 |
| 動作周波数 / Operating frequency / 工作頻率 | 800kHz | WS2812プロトコル / WS2812 protocol / WS2812協定 |


## リファレンス / API Reference / API參考

### コンストラクタ / Constructor / 建構函式

```cpp
WS2812(uint8_t pin, uint8_t xSize, uint8_t ySize, uint8_t xPanelCount = 1, uint8_t yPanelCount = 1)
```

| パラメータ / Parameter / 參數 | 型 / Type / 型別 | 説明 / Description / 說明 |
|---|---|---|
| pin | uint8_t | 出力GPIO番号 / Output GPIO pin number / 輸出GPIO腳位號碼 |
| xSize | uint8_t | 1枚のパネルの幅（ピクセル） / Width of one panel (pixels) / 一片面板的寬度（像素） |
| ySize | uint8_t | 1枚のパネルの高さ（ピクセル） / Height of one panel (pixels) / 一片面板的高度（像素） |
| xPanelCount | uint8_t | パネルの横配置数 / Number of panels horizontally / 面板水平配置數量 |
| yPanelCount | uint8_t | パネルの縦配置数 / Number of panels vertically / 面板垂直配置數量 |

- 800kHzでPIO/SMを初期化し、VRAMを0で確保 / Initializes PIO/SM at 800kHz and allocates VRAM with 0 / 以800kHz初始化PIO/SM並將VRAM配置為0

### 基本制御メソッド / Basic Control Methods / 基本控制方法

| メソッド / Method / 方法 | 説明 / Description / 說明 |
|---|---|
| `void Reset()` | 次フレーム送出前のラッチ確保（>50µs Low）/ Frame latch preparation (>50µs Low) / 畫面鎖存準備（>50µs Low） |
| `void Keep()` | アイドル状態でラインをHigh維持 / Keep line High in idle state / 閒置狀態下保持線路為High |
| `void Clear(uint32_t grb = 0)` | VRAM全体を指定色で塗る / Fill entire VRAM with specified color / 用指定顏色填滿整個VRAM |

### 直接描画メソッド / Direct Drawing Methods / 直接繪圖方法

| メソッド / Method / 方法 | 説明 / Description / 說明 |
|---|---|
| `void setColorDirect(uint8_t r, uint8_t g, uint8_t b)` | 1ピクセル分を即時送信（VRAMを使わない） / Send 1 pixel immediately (without VRAM) / 立即發送1像素（不使用VRAM） |
| `void setColorDirect(uint32_t grb)` | 1ピクセル分を即時送信（VRAMを使わない） / Send 1 pixel immediately (without VRAM) / 立即發送1像素（不使用VRAM） |
| `void fillRandomColorDirect()` | デモ用のランダム色を即時送信 / Send random colors for demo / 發送隨機顏色作為演示 |

### VRAM操作メソッド / VRAM Manipulation Methods / VRAM操作方法

| メソッド / Method / 方法 | 説明 / Description / 說明 |
|---|---|
| `void SetPixel(uint16_t x, uint16_t y, uint32_t grb)` | VRAMの1ピクセルを書き換え（範囲外は無視） / Set 1 pixel in VRAM (ignores out-of-bounds) / 設定VRAM中的1像素（忽略超出範圍） |
| `void DrawPanelBorder(uint8_t panelX, uint8_t panelY, uint32_t grb)` | 指定パネルの外枠をVRAMへ描画 / Draw border of specified panel to VRAM / 將指定面板的邊框繪製到VRAM |
| `void DrawRandomBorders()` | 全パネルの外枠にランダム色を描画 / Draw random colored borders for all panels / 為所有面板繪製隨機顏色邊框 |

### 画面送出メソッド / Display Output Methods / 顯示輸出方法

| メソッド / Method / 方法 | パラメータ / Parameters / 參數 | 説明 / Description / 說明 |
|---|---|---|
| `void ScanPanel(...)` | `posX, posY, serpentine=false, leftToRight=true` | 1パネル分をVRAMからLEDに送出 / Send 1 panel from VRAM to LED / 將1面板從VRAM發送到LED |
| `void ScanBuffer(...)` | `serpentine=false, leftToRight=true` | 全パネルをVRAMからLEDに送出 / Send all panels from VRAM to LED / 將所有面板從VRAM發送到LED |

#### 画面送出パラメータ / Display Output Parameters / 顯示輸出參數

| パラメータ / Parameter / 參數 | 説明 / Description / 說明 |
|---|---|
| serpentine | 千鳥配線対応。trueなら奇数行で左右反転 / Serpentine wiring support. Reverse odd rows if true / 蛇形配線支援。如果為true則反轉奇數行 |
| leftToRight | 基準の走査方向（偶数行） / Base scan direction (even rows) / 基準掃描方向（偶數行） |

### 高度な描画メソッド / Advanced Drawing Methods / 進階繪圖方法

#### DrawBuffer メソッド / DrawBuffer Method / DrawBuffer方法

```cpp
void DrawBuffer(const uint32_t pattern[], uint8_t width, uint8_t height, 
                uint8_t X, uint8_t Y, uint32_t colorReplace, bool isOverlay)
```

任意のパターン配列をVRAMへ描画する高度なメソッド。色置換やオーバーレイ描画に対応。  
Advanced method to draw arbitrary pattern arrays to VRAM. Supports color replacement and overlay drawing.  
將任意圖案陣列繪製到VRAM的進階方法。支援顏色替換和覆蓋繪圖。

| パラメータ / Parameter / 參數 | 型 / Type / 型別 | 説明 / Description / 說明 |
|---|---|---|
| pattern | const uint32_t[] | 描画元のピクセル配列（フラット）/ Source pixel array (flat) / 來源像素陣列（平面） |
| width | uint8_t | patternの幅（ピクセル）/ Pattern width (pixels) / 圖案寬度（像素） |
| height | uint8_t | patternの高さ（ピクセル）/ Pattern height (pixels) / 圖案高度（像素） |
| X | uint8_t | VRAM貼り付け先のX座標 / VRAM destination X coordinate / VRAM目標X座標 |
| Y | uint8_t | VRAM貼り付け先のY座標 / VRAM destination Y coordinate / VRAM目標Y座標 |
| colorReplace | uint32_t | 色置換モード（詳細は下表）/ Color replacement mode (see table below) / 顏色替換模式（詳見下表） |
| isOverlay | bool | オーバーレイモード（詳細は下表）/ Overlay mode (see table below) / 覆蓋模式（詳見下表） |

##### 色置換モード / Color Replacement Mode / 顏色替換模式

| colorReplace値 / Value / 值 | 動作 / Behavior / 行為 |
|---|---|
| 0 | 置換なし。patternの色をそのまま使用 / No replacement. Use pattern colors as-is / 不替換。按原樣使用圖案顏色 |
| 0以外 / Non-zero / 非零 | patternの非0ピクセルをすべてこの色に置換 / Replace all non-zero pixels in pattern with this color / 將圖案中所有非零像素替換為此顏色 |

##### オーバーレイモード / Overlay Mode / 覆蓋模式

| isOverlay値 / Value / 值 | 動作 / Behavior / 行為 |
|---|---|
| true | 黒(0x000000)を透明として扱い、該当ピクセルはVRAM変更しない / Treat black (0x000000) as transparent, don't modify VRAM for those pixels / 將黑色(0x000000)視為透明，不修改這些像素的VRAM |
| false | 黒も不透明として扱い、VRAMを0で上書き / Treat black as opaque, overwrite VRAM with 0 / 將黑色視為不透明，用0覆寫VRAM |

##### データ形式 / Data Format / 資料格式

- 色形式 / Color format / 顏色格式: `0x00GGRRBB` (GRB順、上位8bit未使用 / GRB order, upper 8 bits unused / GRB順序，高8位元未使用)
- インデックス / Index / 索引: `pattern[py * width + px]` (row-major順 / row-major order / row-major順序)


## 使用例 / Usage Example / 使用範例

### 基本的な使用方法 / Basic Usage / 基本使用方法

```cpp
#include "WS2812.h"

// 初期化（GPIO 2、16x16パネル1枚）
// Initialize (GPIO 2, single 16x16 panel)
// 初始化（GPIO 2，單一16x16面板）
WS2812 led_matrix(2, 16, 16);

void setup() {
    // LEDのリセット / Reset LEDs / 重置LED
    led_matrix.Reset();
    
    // VRAMのクリア / Clear VRAM / 清除VRAM
    led_matrix.Clear(0);
    
    // クリアしたVRAMをLEDに転送 / Transfer cleared VRAM to LEDs / 將清除的VRAM傳輸到LED
    led_matrix.ScanBuffer();
}

void main_loop() {
    // パターンをVRAMに描画 / Draw pattern to VRAM / 將圖案繪製到VRAM
    led_matrix.DrawBuffer(pattern_data, 16, 16, 0, 0, 0, false);
    
    // 左上に緑の点を描画 / Draw green dot at top-left / 在左上角繪製綠點
    led_matrix.SetPixel(0, 0, 0xFF0000);  // GRB形式でGreen
    
    // VRAMの内容をLEDに転送 / Transfer VRAM content to LEDs / 將VRAM內容傳輸到LED
    led_matrix.ScanBuffer();
}
```

### 色の指定方法 / Color Specification / 顏色指定方法

| 色 / Color / 顏色 | GRB値 / GRB Value / GRB值 | RGB値 / RGB Value / RGB值 |
|---|---|---|
| 赤 / Red / 紅 | 0x00FF00 | 0xFF0000 |
| 緑 / Green / 綠 | 0xFF0000 | 0x00FF00 |
| 青 / Blue / 藍 | 0x0000FF | 0x0000FF |
| 白 / White / 白 | 0xFFFFFF | 0xFFFFFF |
| 黒 / Black / 黑 | 0x000000 | 0x000000 |

### 千鳥配線対応 / Serpentine Wiring Support / 蛇形配線支援

```cpp
// 千鳥配線のパネルの場合 / For serpentine wired panels / 對於蛇形配線面板
led_matrix.ScanBuffer(true, true);   // serpentine=true, leftToRight=true

// 通常配線の場合 / For normal wiring / 對於正常配線
led_matrix.ScanBuffer(false, true);  // serpentine=false, leftToRight=true
```













