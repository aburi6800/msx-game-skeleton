// License:MIT License
// copyright-holders:Hitoshi Iwai

#include <msx.h>
#include <msx/gfx.h>
#include "game_common.h"

#ifndef GAME_H
#define GAME_H


/*
 * 定数
 */
#define ENEMY_BULLET_MAX    20              // 画面中の最大敵弾数
#define ENEMY_BULLET_IDX    10              // 敵弾のキャラクタアトリビュートの開始インデックス
#define MAPDATA_SIZE        SCREENBUFF_SIZE * 2 // マップデータサイズ

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
 * キャラクタータイプ
 */
typedef enum {
    TYPE_NONE,                              // なし（重ね合わせの2枚目などでも使う）
    TYPE_PLAYER0,                           // プレイヤー（1枚目）
    TYPE_PLAYER1,                           // プレイヤー（2枚目）
    TYPE_P_BULLET,                          // プレイヤー弾
    TYPE_ENEMY,                             // 敵
    TYPE_E_BULLET                           // 敵弾
} CharacterType_t;

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
 * キャラクタアトリビュート構造体
 */
typedef struct {
    SpriteAttr_t attr;
    uint8_t plane;
    uint16_t x;
    uint16_t y;
    uint8_t direction;
    int vx;
    int vy;
    uint8_t speed;
    uint8_t type;
    uint8_t count;
} CharacterAttr_t;

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

/*
 * ゲーム画面描画
 * H.TIMI割り込みで呼ばれる
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void draw_game() __naked __z88dk_fastcall;

/*
 * ゲーム処理
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void update_game();

#endif
