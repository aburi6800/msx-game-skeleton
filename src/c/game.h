// License:MIT License
// copyright-holders:Hitoshi Iwai

#include <msx.h>
#include <msx/gfx.h>
#include "game_common.h"

#ifndef GAME_H
#define GAME_H

/*
 * ゲーム状態Enum
 */
typedef enum {
    STATE_TITLE,                            // タイトル
    STATE_START,                            // スタート
    STATE_GAME,                             // ゲームメイン
    STATE_CLEAR,                            // クリア
    STATE_MISS,                             // ミス
    STATE_OVER                              // ゲームオーバー
} GameState_t;

/*
 * ゲーム情報構造体
 */
typedef struct {
    GameState_t state;                      // ゲームの状態
    uint8_t round[1];                       // ラウンド (BCD値)
    uint8_t score[3];                       // スコア (BCD値)
    uint16_t tick;                          // ゲーム経過時間
} Game_t;

/*
 * キャラクタ情報構造体
 */
typedef struct {
    SpriteAttr_t attr;
    uint8_t plane;
    uint8_t x;
    uint8_t y;
    uint8_t direction;
    int vx;
    int vy;
    uint8_t speed;
    uint8_t type;
} CharacterAttr_t;


/*
 * ゲームタイトル
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void game_title();

/*
 * ゲーム開始処理
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void game_start();

/*
 * ゲームメイン処理
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void game_main();

/*
 * ゲームクリア処理
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void game_clear();

/*
 * ゲームミス処理
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void game_miss();

/*
 * ゲームオーバー処理
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void game_over();

/*
 * ゲーム状態変更
 *
 * args:
 * - state          GameState_t ゲーム状態
 *
 * return:
 * - void
 */
void change_game_state(GameState_t s);

#endif
