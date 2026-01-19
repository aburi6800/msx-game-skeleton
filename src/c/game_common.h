// License:MIT License
// copyright-holders:Hitoshi Iwai

#include <msx.h>
#include <msx/gfx.h>
#include "const.h"

#ifndef GAME_COMMON_H
#define GAME_COMMON_H

// フレームレート
#define FRAME_RATE          1       // 0=1/60、1=1/30, 2=1/15

// スクリーンバッファ定数
#define SCREENBUFF_WIDTH    32
#define SCREENBUFF_HEIGHT   24
#define SCREENBUFF_SIZE     SCREENBUFF_WIDTH * SCREENBUFF_HEIGHT

// キャラクタコード定数
#define CHR_SPACE           0x20


/*
 * スプライトアトリビュート構造体
 */
typedef struct {
    uint8_t y;
    uint8_t x;
    uint8_t ptn;
    uint8_t col;
} SpriteAttr_t;

/*
 * 操作情報構造体
 */
typedef struct {
    uint8_t stick;
    uint8_t strig_a;
    uint8_t strig_b;
} Control_t;


/*
 * スプライトアトリビュート配列
 */
extern SpriteAttr_t spriteAttrTbl[];

/*
 * スプライトプレーンテーブル
 */
extern uint8_t spritePlaneTbl[];

/*
 * スクリーンバッファ
 */
extern uint8_t screenBuffer[];

// 入力情報
extern uint8_t INPUT_STICK;
extern uint8_t INPUT_STRIGA;
extern uint8_t INPUT_STRIGB;

// 方向テーブル
extern uint8_t directionTbl[];

// 象限ごとの DP増分値
extern uint8_t DP_TAB[4];

// XX を X差で取るか Y差で取るか
extern uint8_t XX_SRC[4];

// YY を X差で取るか Y差で取るか
extern uint8_t YY_SRC[4];

// 移動量テーブル
extern int vx[];
extern int vy[];

// VSYNC処理実行フラグ
extern bool vsync_exec;
extern uint8_t vsync_count;


/*
 * ゲーム初期化
 * 各プログラムで実装する
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void boot_game();

/*
 * ゲーム処理
 * 各プログラムで実装する
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void update_game();

/*
 * ゲーム画面描画
 * H.TIMI割り込みで呼ばれる
 * 各プログラムで実装する
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void draw_game();

/*
 * スクリーンバッファクリア
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void clear_screenbuffer();

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
void write_screenBuffer(uint8_t x, uint8_t y, uint8_t *data);

/*
 * スプライトプレーンテーブル初期化
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void init_spritePlainTable();

/*
 * スプライトプレーンテーブルシャッフル
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void shuffle_spritePlainTable();

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
void set_spriteAttrTable(uint8_t plane, SpriteAttr_t attr);

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
void add_bcd(uint16_t addValue, uint8_t *distAddr, uint8_t distBytes) __naked;

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
void write_screenBuffer_bcd(uint8_t *buffAddr, uint8_t buffOffset, uint8_t *dataAddr, uint8_t dataBytes) __naked;

/*
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
void get_control() __naked __FASTCALL__;

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
uint8_t get_direction(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

#endif
