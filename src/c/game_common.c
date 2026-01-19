// License:MIT License
// copyright-holders:Hitoshi Iwai

#include <msx.h>
#include <msx/gfx.h>
#include "const.h"
#include "game_common.h"


// スプライトアトリビュートテーブル
SpriteAttr_t spriteAttrTbl[32];

// スプライトプレーンテーブル
uint8_t spritePlaneTbl[32];

// スクリーンバッファ
uint8_t screenBuffer[SCREENBUFF_SIZE];

// 入力情報
uint8_t INPUT_STICK;
uint8_t INPUT_STRIGA;
uint8_t INPUT_STRIGB;

// 方向テーブル
uint8_t directionTbl[] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    5,3,2,1,1,1,1,1,1,1,1,1,1,1,1,1,
    5,3,3,2,2,1,1,1,1,1,1,1,1,1,1,1,
    5,4,3,3,2,2,2,2,1,1,1,1,1,1,1,1,
    5,4,3,3,3,2,2,2,2,2,1,1,1,1,1,1,
    5,4,4,3,3,3,2,2,2,2,2,2,2,1,1,1,
    5,4,4,3,3,3,3,2,2,2,2,2,2,2,2,1,
    5,4,4,3,3,3,3,3,2,2,2,2,2,2,2,2,
    5,4,4,4,3,3,3,3,3,2,2,2,2,2,2,2,
    5,4,4,4,3,3,3,3,3,3,2,2,2,2,2,2,
    5,4,4,4,4,3,3,3,3,3,3,2,2,2,2,2,
    5,4,4,4,4,3,3,3,3,3,3,3,2,2,2,2,
    5,4,4,4,4,3,3,3,3,3,3,3,3,2,2,2,
    5,4,4,4,4,4,3,3,3,3,3,3,3,3,2,2,
    5,4,4,4,4,4,3,3,3,3,3,3,3,3,3,2,
    5,4,4,4,4,4,4,3,3,3,3,3,3,3,3,3
};

// 移動量テーブル
// 実数で扱うため、256倍しておく
//int vx[] = { 0.00,  0.00,  0.38,  0.75,  0.92,  1.00,  0.92,  0.75,  0.38,  0.00, -0.38, -0.75, -0.92, -1.00, -0.92, -0.75, -0.38 };
//int vy[] = { 0.00, -1.00, -0.92, -0.75, -0.38,  0.00,  0.38,  0.75,  0.92,  1.00,  0.92,  0.75,  0.38,  0.00, -0.38, -0.75, -0.92 };
int vx[] =   {    0,     0,    97,   192,   235,   256,   235,   192,    97,     0,   -97,  -192,  -235,  -256,  -235,  -192,   -97 };
int vy[] =   {    0,  -256,  -235,  -192,   -97,     0,    97,   192,   235,   256,   235,   192,    97,     0,   -97,  -192,  -235 };

// VSYNC処理実行フラグ
bool vsync_exec = false;
uint8_t vsync_count = FRAME_RATE;


/*
 * スクリーンバッファクリア
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void clear_screenbuffer()
{
    for (uint16_t i = 0; i < SCREENBUFF_SIZE; i++) {
        screenBuffer[i] = CHR_SPACE;
    }
}

/*
 * スクリーンバッファ書き込み
 *
 * args:
 * - x              uint8_t     書き込み先のX座標
 * - y              uint8_t     書き込み先のY座標
 * - *data          uint8_t     書き込むデータのポインタ
 *
 * return:
 * - void
 */
void write_screenBuffer(uint8_t x, uint8_t y, uint8_t *data)
{
    while (*data != 0x00) {
        screenBuffer[y*32 + x] = *data;
        data++;
        x++;
    }
}

/*
 * スプライトプレーンテーブル初期化
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void init_spritePlainTable()
{
    // プレーンテーブル初期化
    for (uint8_t i = 0; i < 32; i++) {
        spritePlaneTbl[i] = i;
    }
}

/*
 * スプライトプレーンテーブルシャッフル
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void shuffle_spritePlainTable()
{
    // スプライトプレーン0,1はプレイヤー用の固定なので2から処理する
    uint8_t w_plane = spritePlaneTbl[2];

    for (uint8_t i = 2; i < 32; i++) {
        w_plane = w_plane + 11;
        if (w_plane > 31) {
            w_plane = w_plane - 30;
        }
        spritePlaneTbl[i] = w_plane;
    }
}

/*
 * スプライトアトリビュートテーブル設定
 *
 * args:
 * - plane          uint8_t      スプライトプレーン番号
 * - attr           SpriteAttr_t スプライトアトリビュート構造体
 *
 * return:
 * - void
 */
void set_spriteAttrTable(uint8_t plane, SpriteAttr_t attr)
{
    spriteAttrTbl[spritePlaneTbl[plane]] = attr;
}

/*
 * BCD値加算
 * - 対象データに対し引数で与えた16ビットのBCD値を加算する。
 *
 * args:
 * - addValue       uint16_t    加算値
 * - *distAddr      uint8_t     加算対象データの先頭アドレス
 * - distBytes      uint8_t     加算対象データのバイト数
 *
 * return:
 * - void
 */
void add_bcd(uint16_t addValue, uint8_t *distAddr, uint8_t distBytes) __naked
{
#ifndef __INTELLISENSE__
// clang-format off
__asm

    ; SPに+2してリターンアドレスをスキップ
    LD      HL, 2
    ADD     HL, SP

    ; 第3引数(加算対象データバイト数)を取得
    LD      B, (HL)                 ; B <- 加算対象データバイト数
    INC     HL
    INC     HL

    ; 第2引数(加算対象データアドレス)を取得
    LD      E, (HL)
    INC     HL
    LD      D, (HL)
    INC     HL
    PUSH    DE                      ; スタックに退避

    ; 第1引数(加算値)を取得
    LD      E, (HL)                 ; DE <- 加算値
    INC     HL
    LD      D, (HL)

    ; レジスタ設定
    POP     HL                      ; HL <- 加算対象データアドレス

    ; 加算対象データバイト数により処理開始アドレス設定
    DEC     B                       ; 加算対象データバイト数を1減算 (1byte=0、2byte=1、3byte=2)

    LD      A, B                    ; A <- B
    ADD     A, L                    ; HL=HL+A
    JR      NC, add_bcd_L1
    INC     H

add_bcd_L1:
    LD      L, A

    ; 1〜2桁目の加算
	LD      A, E                    ; AレジスタにEレジスタの値をロード
    ADD     A, (HL)                 ; Aレジスタの値に(HL)の値を加算
    DAA						        ; Aレジスタの値を内部10進に補正
    						        ;   桁溢れした場合はここでキャリーフラグが立つ
    PUSH    AF                      ; フラグの状態を保存
    LD      (HL), A                 ; Aレジスタの値を(HL)に格納

    LD      A, B                    ; Aレジスタに現在の加算対象データバイト数をロード
    OR      A                       ; ゼロか？
    JR      Z, add_bcd_L2           ; ゼロならadd_bcd_L2へ
    DEC     A                       ; 加算対象データバイト数を1減算
    LD      B, A                    ; Bレジスタに保存

    ; 3〜4桁目の加算
    DEC     HL                      ; 加算対象データアドレスを-1
    POP     AF                      ; フラグの状態を復帰
    LD      A, D                    ; AレジスタにDレジスタの値をロード
    ADC     A, (HL)	                ; Aレジスタの値に(HL)＋キャリーフラグを加算
                                    ;   桁溢れした場合はキャリーフラグが立つが無視する
    DAA                             ; Aレジスタの値を内部10進に補正
    PUSH    AF                      ; フラグの状態を保存
    LD      (HL), A                 ; Aレジスタの値を(HL)に格納

    JR      NC, add_bcd_L2          ; 桁繰り上がりが無ければadd_bcd_L2へ

    LD      A, B                    ; Aレジスタに現在の加算対象データバイト数をロード
    OR      A                       ; ゼロか？
    JR      Z, add_bcd_L2           ; ゼロならadd_bcd_L2へ

    ; 5〜6桁目の加算（桁繰り上がり発生時のみ）
    POP     AF                      ; フラグの状態を復帰
    OR      A                       ; フラグクリア
    DEC     HL                      ; 加算対象データアドレスを-1
    LD      A, (HL)                 ; 5〜6桁目に1加算
    INC     A
    DAA                             ; Aレジスタの値を内部10進に補正
    PUSH    AF                      ; フラグの状態を保存（ダミー）
    LD      (HL), A                 ; Aレジスタの値を(HL)に格納

add_bcd_L2:
    POP     AF

    RET

__endasm
// clang-format on
#endif
}

/*
 * スクリーンバッファ書き込み (BCD値)
 *
 * args:
 * - *buffAddr      uint8_t    スクリーンバッファアドレス
 * - buffOffset     uint8_t    スクリーンバッファオフセット値
 * - *distAddr      uint8_t    表示データアドレス
 * - distBytes      uint8_t    表示データバイト数
 *
 * return:
 * - void
 */
void write_screenBuffer_bcd(uint8_t *buffAddr, uint8_t buffOffset, uint8_t *dataAddr, uint8_t dataBytes) __naked
{
#ifndef __INTELLISENSE__
// clang-format off
__asm

    ; SPに+2してリターンアドレスをスキップ
    LD      HL, 2
    ADD     HL, SP

    ; 第4引数(表示データバイト数)を取得
    LD      B, (HL)                 ; B <- 表示データバイト数
    INC     HL
    INC     HL

    ; 第3引数(表示データアドレス)を取得
    LD      E, (HL)
    INC     HL
    LD      D, (HL)
    INC     HL
    PUSH    DE                      ; スタックに退避

    ; 第2引数(スクリーンバッファオフセット値)を取得
    LD      E, (HL)
    INC     HL
    LD      D, (HL)
    INC     HL
    PUSH    DE                      ; スタックに退避

    ; 第1引数(スクリーンバッファアドレス)を取得
    LD      E, (HL)
    INC     HL
    LD      D, (HL)
    PUSH    DE                      ; スタックに退避

    ; レジスタ設定
    POP     HL                      ; HL <- スクリーンバッファアドレス
    POP     DE                      ; DE <- スクリーンバッファオフセット値
    ADD     HL, DE                  ; HL + DE = 設定先のスクリーンバッファアドレス
    POP     DE                      ; DE <- 表示データアドレス

PRTBCD:
    ; 表示する桁数分、スペースで埋める
    PUSH    BC
    PUSH    DE
    PUSH    HL

    LD      A, B                    ; A <- B (表示データバイト数)
    ADD     A, A                    ; 表示データバイト数*2 で表示文字数を算出
    LD      B, A                    ; B (繰り返し数) = 表示文字数

PRTBCD_L2:
    LD      (HL), $20               ; 表示桁分、スペースを埋める
    INC     HL
    DJNZ    PRTBCD_L2

    DEC     HL
    LD      (HL), $30               ; 末尾はゼロ固定表示

    POP     HL
    POP     DE
    POP     BC

    ; 数値描画
    LD      C,0                     ; ゼロ表示フラグ初期化

PRTBCD_L3:
    LD      A, (DE)                 ; A <- DE (表示データ)
	CALL    PUTBCD                  ; データを表示

    INC     DE                      ; DE = DE + 1 (表示データの次のアドレスが設定される)
    INC     HL                      ; HL = HL + 1 (表示位置を1つ右に移動)

    DJNZ    PRTBCD_L3               ; B = B - 1、ゼロでなければ繰り返す

    RET


    ; 1バイト(2桁)のBCD値描画
PUTBCD:
    ; 上1桁の処理
    PUSH    AF
    SRL     A                       ; Aレジスタの値を4回右シフトして、上位4ビットを取り出す
    SRL     A
    SRL     A
    SRL     A
    CALL    PUTBCD_L1               ; スクリーンバッファにデータ設定

    ; 下1桁の処理
    POP     AF
    INC     HL                      ; HLレジスタの値を1加算(データ表示位置を1つ右に移動)

PUTBCD_L1:
    ; スクリーンバッファにデータを設定
	AND     $0F                     ; 上位4ビットをゼロにする (=下位4ビットの値だけ取り出す)
    OR      A
    JR      NZ, PUTBCD_L2           ; 値がゼロ以外の場合はL2へ

    INC     C                       ; 値がゼロの時はゼロ表示フラグを判定
    DEC     C
    RET     Z                       ; ゼロ表示フラグがOFFの時はゼロ表示せず抜ける

PUTBCD_L2:
    INC     C                       ; 以降のゼロは表示させるので、ゼロ表示フラグを+1
    ADD     A, $30				    ; 値にキャラクタコード&H30('0')を加える
    LD      (HL), A                 ; 仮想画面にデータを設定

    RET

__endasm
// clang-format on
#endif
}

/**
 * 入力情報取得
 * 入力値はINPUT_STICK、INPUT_STRIGAに保存される
 * INPUT_STICKは0～8の値、INPUT_STRIGAはON=1、OFF=0の値となる
 *
 * args:
 * - None
 *
 * return:
 * - void
 */
void get_control() __naked __FASTCALL__
{
#ifndef __INTELLISENSE__
// clang-format off
__asm

    CALL    0x0156                  ; BIOS : KILBUF
    OR      A                       ; キャリーフラグクリア

    ; STICK値取得
    XOR     A                       ; A <- ジョイスティック番号=0 (キーボード)
    CALL    0x00D5                  ; BIOS : GTSTCK
                                    ; - A = 入力値 (0～8)
    OR      A
    JR      NZ, get_control_l1

    LD      A, 1                    ; A <- ジョイスティック番号=1 (パッド1)
    CALL    0x00D5                  ; BIOS : GTSTCK
                                    ; - A = 入力値 (0～8)
get_control_l1:
    LD      (_INPUT_STICK), A       ; 現在の入力値を保存

    ; STRIG値取得(Aボタン)
    XOR     A                       ; A <- ジョイスティック番号=0 (キーボード)
    CALL    0x00D8                  ; BIOS : GTTRIG
                                    ; - 0x00 = 押されていない
                                    ; - 0xFF = 押されている
    LD      D, A
    LD      A, 1                    ; A <- ジョイスティック番号=1 (ジョイスティックポート1 ボタンA)
    CALL    0x00D8                  ; BIOS : GTTRIG
    OR      D
    AND     0x01
    LD (_INPUT_STRIGA), A           ; 現在の入力値を保存

    RET

__endasm
// clang-format on
#endif
}

/*
 * 方向取得処理
 * 方向は上から右周りに1～16で返される
 *
 * args:
 * - x1             uint8_t     開始点のX座標
 * - y1             uint8_t     開始点のY座標
 * - x2             uint8_t     目標点のX座標
 * - y2             uint8_t     目標点のY座標
 *
 * return:
 * - uint8_t        方向値(1～16)
 */
uint8_t get_direction(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    uint8_t XX, YY, DP;
    uint8_t BD;

    x1 = x1 >> 4;
    y1 = y1 >> 4;
    x2 = x2 >> 4;
    y2 = y2 >> 4;

    if (x1 <= x2 && y1 >= y2) {
        XX = y1 - y2;
        YY = x2 - x1;
        DP = 1;
    } else if (x1 <= x2 && y1 <= y2) {
        XX = x2 - x1;
        YY = y2 - y1;
        DP = 5;
    } else if (x1 >= x2 && y1 <= y2) {
        XX = y2 - y1;
        YY = x1 - x2;
        DP = 9;
    } else {
        XX = x1 - x2;
        YY = y1 - y2;
        DP = 13;
    }

    BD = directionTbl[(YY << 4) + XX] + DP;
    if (BD > 16) {
        BD -= 16;
    }

    return BD;
}
