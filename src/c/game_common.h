// License:MIT License
// copyright-holders:Hitoshi Iwai

#include <msx.h>
#include <msx/gfx.h>
#include "const.h"

#ifndef GAME_COMMON_H
#define GAME_COMMON_H

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

/*
 * 入力情報
 */
extern uint8_t INPUT_STICK;
extern uint8_t INPUT_STRIGA;
extern uint8_t INPUT_STRIGB;


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
void add_bcd(uint16_t addValue, uint8_t *distAddr, uint8_t distBytes);

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
void write_screenBuffer_bcd(uint8_t *buffAddr, uint8_t buffOffset, uint8_t *dataAddr, uint8_t dataBytes);

/*
 * 入力情報取得
 *
 * args:
 * - None
 *
 * return:
 * - void
 */
void get_control();

#endif
