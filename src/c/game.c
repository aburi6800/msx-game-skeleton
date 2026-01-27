// License:MIT License
// copyright-holders:Hitoshi Iwai

#include <msx.h>
#include <msx/gfx.h>
#include "game_common.h"
#include "game.h"


// pcgdata.asm : BANK_PATTERN_0への参照
extern uint8_t BANK_PATTERN_0[];

// pcgdata.asm : BANK_COLOR_0への参照
extern uint8_t BANK_COLOR_0[];

// sprdata.asm : SPRITE_PATTERNへの参照
extern uint8_t SPRITE_PATTERN[];


// ゲーム情報
Game_t game;

// 画面中の敵弾数
uint8_t enemyBulletCount = 0;

// キャラクターアトリビュートテーブル
CharacterAttr_t characterAttrTbl[32];

// 敵弾パターンテーブル
uint8_t enemy_bullet_ptn[2] = {12, 16};

// VSYNC処理実行フラグ
bool vsync_exec = false;
uint8_t vsync_count = FRAME_RATE;


// 文字列定義
uint8_t TITLE1[] = "SHOOTING GAME";
uint8_t TITLE2[] = "PUSH START BUTTON";
uint8_t TITLE3[] = "(C)ABURI GAMES 2026";
uint8_t TITLE4[] = "ALL RIGHT RESERVED";

// スコアデータ (BCD値)
unsigned char score[3] = {0x00, 0x00, 0x00};

// プレイヤー移動テーブル
uint8_t playerMoveTbl[9] = { 0,  1,  3,  5,  7,  9, 11, 13, 15};

// 画面上のショット数カウンタ
uint8_t playerBulletCount = 0;

// スクロール用
// 表示データ
uint8_t mapdata[48 * 24 + 1] = {
    'A', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ',
    'B', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2',
    'C', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ',
    'D', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2',
    'E', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ',
    'F', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2',
    'G', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ',
    'H', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2',
    'I', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ',
    'J', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2',
    'K', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ',
    'L', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2',
    'M', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ',
    'N', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2',
    'O', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ',
    'P', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2',
    'Q', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ',
    'R', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2',
    'S', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ',
    'T', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2',
    'U', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ',
    'V', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2',
    'W', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ',
    'X', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2', ' ', '2',
    'A', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ',
    'B', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1',
    'C', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ',
    'D', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1',
    'E', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ',
    'F', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1',
    'G', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ',
    'H', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1',
    'I', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ',
    'J', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1',
    'K', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ',
    'L', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1',
    'M', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ',
    'N', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1',
    'O', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ',
    'P', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1',
    'Q', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ',
    'R', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1',
    'S', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ',
    'T', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1',
    'U', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ',
    'V', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1',
    'W', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ',
    'X', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1', ' ', '1',
    0xff
};

// スクロールカウンタ
uint8_t scroll_step = 0;

// マップデータの転送開始位置
uint16_t mapdata_idx = 24 * 24;

// テスト文字列
uint8_t testText[] = {'G', 'A', 'M', 'E', ' ', 'O', 'V', 'E', 'R'};

/*
 * ゲーム起動処理
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void boot_game()
{
    // 割り込み処理不許可
    vsync_exec = false;
    vsync_count = FRAME_RATE;

    // 画面初期化
    set_color(15, 1, 1);
    set_mangled_mode();
    msx_set_sprite_mode(sprite_large);

    // キークリックスイッチOFF
    *(uint8_t *)MSX_CLIKSW = 0;

    // パターンジェネレータテーブル設定
    vwrite(BANK_PATTERN_0, VRAM_PTN_GENR_TBL1, VRAM_PTN_GENR_SIZE);
    vwrite(BANK_PATTERN_0, VRAM_PTN_GENR_TBL2, VRAM_PTN_GENR_SIZE);
    vwrite(BANK_PATTERN_0, VRAM_PTN_GENR_TBL3, VRAM_PTN_GENR_SIZE);

    // カラーテーブル設定
    vwrite(BANK_COLOR_0, VRAM_COLOR_TBL1, VRAM_COLOR_SIZE);
    vwrite(BANK_COLOR_0, VRAM_COLOR_TBL2, VRAM_COLOR_SIZE);
    vwrite(BANK_COLOR_0, VRAM_COLOR_TBL3, VRAM_COLOR_SIZE);

    // スプライトパターンテーブル設定
    vwrite(SPRITE_PATTERN, VRAM_SPR_PTN_TBL, 32 * 5);

    // H.TIMIフック設定
#ifndef __INTELLISENSE__
#asm
    DI
#endasm;
#endif
    uint8_t *h_timi = (uint8_t *)MSX_H_TIMI;
    uint16_t hook_addr = (uint16_t)&draw_game;
    h_timi[0] = 0xc3; // JP
    h_timi[1] = (uint8_t)(hook_addr & 0xff);
    h_timi[2] = (uint8_t)((hook_addr & 0xff00) >> 8);
#ifndef __INTELLISENSE__
#asm
    EI
#endasm;
#endif

    // スプライトプレーンテーブル初期化
    init_spritePlainTable();

    // ゲーム状態変更
    change_game_state(STATE_TITLE);
}

/*
 * ゲーム状態変更
 *
 * args:
 * - state          GameState_t ゲーム状態
 *
 * return:
 * - void
 */
void change_game_state(GameState_t state)
{
    game.state = state;
    game.tick = 0xFFFF;
}

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
void draw_game() __naked __z88dk_fastcall
{
// clang-format off
#ifndef __INTELLISENSE__
#asm
    // VSYNCカウント
    //   1 = スクロール処理
    //   0 = その他処理
    LD      A, (_vsync_count)
    OR      A
    JR      Z, _draw_game_1

    DEC     A
    LD      (_vsync_count), A
    JR      _do_reverse_scroll_step     ; スクロール

_draw_game_1:
    // VSYNC処理が許可されていないときはreturn
    LD      A, (_vsync_exec)
    OR      A
    RET     Z

_draw_game_2:

    DI

    ; 周辺色変更
;    LD      A, 0x07                     ; COLOR CODE
;    LD      (0xF3EB), A                 ; 0xF3EB = BDRCLR
;    CALL    0x0062                      ; BIOS : CHGCLR

    ; スプライトアトリビュートテーブル更新
    LD      HL, _spriteAttrTbl          ; Source addr
    LD      DE, VRAM_SPR_ATTR_TBL       ; Dist addr
    LD      BC, VRAM_SPR_ATTR_SIZE      ; Length
    CALL    0x005C                      ; BIOS : LDIRVM

    ; パターンネームテーブル更新
    LD      HL, _testText               ; Source addr
    LD      DE, VRAM_PTN_NAME_TBL + 8 + 10 * 32 ; Dist addr
    LD      BC, 9                       ; Length
    CALL    0x005C                      ; BIOS : LDIRVM

    ; VSYNC処理を不許可にする
    XOR     A
    LD      (_vsync_exec), A
    LD      A, FRAME_RATE
    LD      (_vsync_count), A

    ; 周辺色戻す
;    LD      A, 0x01                    ; COLOR CODE
;    LD      (0xF3EB), A                ; 0xF3EB = BDRCLR
;    CALL    0x0062                     ; BIOS : CHGCLR

    EI

    RET

;----------------------------------
; reverse_scroll
;----------------------------------
_do_reverse_scroll_step:
    DI

    ; HL = src addr
    LD      HL, _mapdata
    LD      BC, (_mapdata_idx)          ; map offset
    ADD     HL, BC

    ; DE = dst VRAM addr
    LD      DE, 0x1800

    ; B = loop counter(line)
    LD      B, 24                       ; row num

_loop:
    ; ---- SET WRITE ----
    LD      A, (0x0007)                 ; get write port #0 address
    LD      C, A
    INC     C                           ; port #1 address

    LD      A, E                        ; set dist addr
    OUT     (C), A
    LD      A, D
    OR      0x40                        ; write mode
    OUT     (C), A
    PUSH    BC

    LD      B, 24                       ; col num

    ; ---- WRITE ----
    DEC     C                           ; port #0 address
_loop_1:
    OUTI
    JP      NZ, _loop_1

    EX      DE, HL
    ADD     HL, 32
    EX      DE, HL


; 末端まで行ったか
    LD      A, (HL)
    CP      0xFF
    JP      NZ, _loop_next

_reset_mapdata_idx:
    LD      HL, _mapdata                ; set first address


_loop_next:
    POP     BC
    DJNZ    _loop

    LD      HL, (_mapdata_idx)
    LD      A, H
    OR      L
    JR      Z, _reset_mapdata_idx2
    ADD     HL, -24
    LD      (_mapdata_idx), HL

    EI

    RET

_reset_mapdata_idx2:
    LD      HL, 576 * 2 - 24            ; set last line
    LD      (_mapdata_idx), HL

    RET

#endasm;
#endif
// clang-format on
}


/*
 * ゲームタイトル処理
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void game_title()
{
    if (game.tick == 0) {
        write_screenBuffer(9, 6, TITLE1);
        write_screenBuffer(7, 15, TITLE2);
        write_screenBuffer(6, 21, TITLE3);
        write_screenBuffer(6, 22, TITLE4);
    }

    if (INPUT_STRIGA) {
        // ゲーム状態変更
        change_game_state(STATE_START);
    }

}

/*
 * ゲーム開始処理
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void game_start()
{
    // キャラクター初期化
    for (uint8_t i = 0; i < 32; i++) {
        characterAttrTbl[i].type = TYPE_NONE;
        characterAttrTbl[i].plane = i;
        characterAttrTbl[i].attr.x = 0;
        characterAttrTbl[i].attr.y = 192;
        characterAttrTbl[i].attr.col = 0;
        characterAttrTbl[i].attr.ptn = 0;
        characterAttrTbl[i].x = characterAttrTbl[i].attr.x << 8;
        characterAttrTbl[i].y = characterAttrTbl[i].attr.y << 8;
    }

    // プレイヤー
    characterAttrTbl[0].type = TYPE_PLAYER0;
    characterAttrTbl[0].attr.x = 120;
    characterAttrTbl[0].attr.y = 88;
    characterAttrTbl[0].attr.col = 10;
    characterAttrTbl[0].attr.ptn = 0;
    characterAttrTbl[0].x = characterAttrTbl[0].attr.x << 8;
    characterAttrTbl[0].y = characterAttrTbl[0].attr.y << 8;
    characterAttrTbl[0].vx = 0;
    characterAttrTbl[0].vy = 0;

    characterAttrTbl[1].type = TYPE_PLAYER1;  // 2枚目なのでNONEにしたい
    characterAttrTbl[1].attr.x = 120;
    characterAttrTbl[1].attr.y = 88;
    characterAttrTbl[1].attr.col = 15;
    characterAttrTbl[1].attr.ptn = 4;
    characterAttrTbl[1].x = characterAttrTbl[1].attr.x << 8;
    characterAttrTbl[1].y = characterAttrTbl[1].attr.y << 8;
    characterAttrTbl[1].vx = 0;
    characterAttrTbl[1].vy = 0;

    // 敵
    uint8_t idx = ENEMY_IDX;
    characterAttrTbl[idx].type = TYPE_ENEMY;
    characterAttrTbl[idx].attr.x = 48;
    characterAttrTbl[idx].attr.y = 80;
    characterAttrTbl[idx].attr.col = 13;
    characterAttrTbl[idx].attr.ptn = 0;
    characterAttrTbl[idx].x = characterAttrTbl[2].attr.x << 8;
    characterAttrTbl[idx].y = characterAttrTbl[2].attr.y << 8;
    characterAttrTbl[idx].vx = 0;
    characterAttrTbl[idx].vy = 2;
    characterAttrTbl[idx].count = 10;

    idx++;
    characterAttrTbl[idx].type = TYPE_ENEMY;
    characterAttrTbl[idx].attr.x = 72;
    characterAttrTbl[idx].attr.y = 82;
    characterAttrTbl[idx].attr.col = 12;
    characterAttrTbl[idx].attr.ptn = 0;
    characterAttrTbl[idx].x = characterAttrTbl[3].attr.x << 8;
    characterAttrTbl[idx].y = characterAttrTbl[3].attr.y << 8;
    characterAttrTbl[idx].vx = 0;
    characterAttrTbl[idx].vy = 1;
    characterAttrTbl[idx].count = 15;

    idx++;
    characterAttrTbl[idx].type = TYPE_ENEMY;
    characterAttrTbl[idx].attr.x = 96;
    characterAttrTbl[idx].attr.y = 84;
    characterAttrTbl[idx].attr.col = 10;
    characterAttrTbl[idx].attr.ptn = 0;
    characterAttrTbl[idx].x = characterAttrTbl[4].attr.x << 8;
    characterAttrTbl[idx].y = characterAttrTbl[4].attr.y << 8;
    characterAttrTbl[idx].vx = 0;
    characterAttrTbl[idx].vy = 2;
    characterAttrTbl[idx].count = 22;

    idx++;
    characterAttrTbl[idx].type = TYPE_ENEMY;
    characterAttrTbl[idx].attr.x = 144;
    characterAttrTbl[idx].attr.y = 90;
    characterAttrTbl[idx].attr.col = 7;
    characterAttrTbl[idx].attr.ptn = 0;
    characterAttrTbl[idx].x = characterAttrTbl[5].attr.x << 8;
    characterAttrTbl[idx].y = characterAttrTbl[5].attr.y << 8;
    characterAttrTbl[idx].vx = 0;
    characterAttrTbl[idx].vy = 2;
    characterAttrTbl[idx].count = 29;

    idx++;
    characterAttrTbl[idx].type = TYPE_ENEMY;
    characterAttrTbl[idx].attr.x = 168;
    characterAttrTbl[idx].attr.y = 92;
    characterAttrTbl[idx].attr.col = 4;
    characterAttrTbl[idx].attr.ptn = 0;
    characterAttrTbl[idx].x = characterAttrTbl[6].attr.x << 8;
    characterAttrTbl[idx].y = characterAttrTbl[6].attr.y << 8;
    characterAttrTbl[idx].vx = 0;
    characterAttrTbl[idx].vy = 1;
    characterAttrTbl[idx].count = 36;

    idx++;
    characterAttrTbl[idx].type = TYPE_ENEMY;
    characterAttrTbl[idx].attr.x = 192;
    characterAttrTbl[idx].attr.y = 94;
    characterAttrTbl[idx].attr.col = 3;
    characterAttrTbl[idx].attr.ptn = 0;
    characterAttrTbl[idx].x = characterAttrTbl[7].attr.x << 8;
    characterAttrTbl[idx].y = characterAttrTbl[7].attr.y << 8;
    characterAttrTbl[idx].vx = 0;
    characterAttrTbl[idx].vy = 2;
    characterAttrTbl[idx].count = 41;

    // 画面上のショット数カウンタをリセット
    playerBulletCount = 0;

    // ゲーム状態変更
    change_game_state(STATE_GAME);
}


/*
 * ゲームメイン処理
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void game_main()
{
    CharacterAttr_t *character;

    for (uint8_t i = 0; i < 32; i++) {
        character = &characterAttrTbl[i];

        // プレイヤー
        if (character->type == TYPE_PLAYER0) {
            get_control();

            if (INPUT_STICK) {
                character->x += (vx[playerMoveTbl[INPUT_STICK]] * 6);
                character->y += (vy[playerMoveTbl[INPUT_STICK]] * 6);
                character->attr.x = character->x >> 8;
                character->attr.y = character->y >> 8;

                if (character->attr.x < 8) {
                    character->attr.x = 8;
                    character->x = character->attr.x << 8;
                } else if (character->attr.x > 255 - 16 - 8) {
                    character->attr.x = 255 - 16 - 8;
                    character->x = character->attr.x << 8;
                } else if (character->attr.y < 8) {
                    character->attr.y = 8;
                    character->y = character->attr.y << 8;
                } else if (character->attr.y > 192 - 16 - 8) {
                    character->attr.y = 192 - 16 - 8;
                    character->y = character->attr.y << 8;
                }

                characterAttrTbl[1].attr.x = character->attr.x;
                characterAttrTbl[1].attr.y = character->attr.y;
            }

            if (INPUT_STRIGA) {
                if (playerBulletCount < 6) {
                    for (uint8_t i = 0; i < PLAYER_BULLET_MAX; i++) {
                        uint8_t idx = PLAYER_BULLET_IDX + i;
                        if (characterAttrTbl[idx].type == TYPE_NONE) {
                            playerBulletCount++;
                            CharacterAttr_t *e_bullet = &characterAttrTbl[idx]; 
                            e_bullet->type = TYPE_P_BULLET;
                            e_bullet->attr.x = character->attr.x;
                            e_bullet->attr.y = character->attr.y;
                            e_bullet->attr.col  = 14;
                            e_bullet->attr.ptn  = 8;
                            e_bullet->x = character->attr.x << 8;
                            e_bullet->y = character->attr.y << 8;
                            e_bullet->vx = 0;
                            e_bullet->vy = (vy[1] * 14); // 14 = speed
                            break;
                        }
                    }
                }
            }

        // プレイヤー弾
        } else if (character->type == TYPE_P_BULLET) {
            character->y += character->vy;
            character->attr.y = character->y >> 8;
            if (character->attr.y < 14) {
                character->type = TYPE_NONE;
                character->attr.x = 0;
                character->attr.y = 193;

                playerBulletCount--;
            }

        // 敵
        } else if (character->type == TYPE_ENEMY) {
            // 移動
            // 手を抜いてattrのx,yを直接更新しているが、本来はx,yを更新後にattr.x,yを算出すべき
//            if (character->vy != 0) {
                character->attr.y += character->vy;
/*
                if (character->attr.y < 8) {
                    character->attr.y = 8;
                    character->vy = character->vy * -1;
                }
*/
                if (character->attr.y > 176) {
                    character->attr.y = 0;
                    character->attr.x = get_rnd() % 176;
                    character->vy = get_rnd() % 5 + 1;
                }
                // 内部座標も入れておく
                character->x = character->attr.x << 8;
                character->y = character->attr.y << 8;
//            }
            // 弾を撃つか？
            if (character->count-- == 0) {
                // 次の発射までのカウンタリセット
                character->count = 40;
                // 弾を生成できる？
                if (enemyBulletCount < ENEMY_BULLET_MAX) {
                    // 弾を生成
                    for (uint8_t i = 0; i < ENEMY_BULLET_MAX; i++) {
                        uint8_t idx = ENEMY_BULLET_IDX + i;
                        if (characterAttrTbl[idx].type == TYPE_NONE) {
                            enemyBulletCount++;
                            CharacterAttr_t *e_bullet = &characterAttrTbl[idx]; 
                            e_bullet->type = TYPE_E_BULLET;
                            e_bullet->attr.x = character->attr.x;
                            e_bullet->attr.y = character->attr.y;
                            e_bullet->attr.col  = 7;
                            e_bullet->attr.ptn  = enemy_bullet_ptn[game.tick % 2];
                            e_bullet->x = character->attr.x << 8;
                            e_bullet->y = character->attr.y << 8;
                            uint8_t d = get_direction(character->attr.x, character->attr.y, characterAttrTbl[0].attr.x, characterAttrTbl[0].attr.y);
//                            characterAttrTbl[i].speed = 8;
                            e_bullet->vx = (vx[d] * 8); // 8 = speed
                            e_bullet->vy = (vy[d] * 8); // 8 = speed
                            break;
                        }
                    }
                }
            }

        // 敵弾
        } else if (character->type == TYPE_E_BULLET) {
            character->x += character->vx;
            character->y += character->vy;
            character->attr.ptn  = enemy_bullet_ptn[game.tick % 2];
            character->attr.x = character->x >> 8;
            character->attr.y = character->y >> 8;
            if (character->attr.x < 8 || character->attr.x > 255 - 16 - 8 || character->attr.y < 8 || character->attr.y > 192 - 16 - 8) {
                character->type = TYPE_NONE;
                character->attr.x = 0;
                character->attr.y = 193;

                enemyBulletCount--;
            }
            // プレイヤーとの当たり判定もここで行う
        }
    }

}

/*
 * ゲームクリア処理
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void game_clear()
{

}

/*
 * ゲームミス処理
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void game_miss()
{

}

/*
 * ゲームオーバー処理
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void game_over()
{

}

/*
 * ゲーム処理
 *
 * args:
 * - none
 *
 * return:
 * - void
 */
void update_game()
{
    // プレーンテーブルをシャッフル
    shuffle_spritePlainTable();

    // キャラクターのアトリビュート情報をスプライトアトリビュートに設定
    for (uint8_t i = 0; i < 32; i++) {
        set_spriteAttrTable(characterAttrTbl[i].plane, characterAttrTbl[i].attr);
    }

    // 入力情報取得
    get_control();

    // tick値インクリメント
    game.tick++;

    switch (game.state) {
        case STATE_TITLE:
            game_title();
            break;
        case STATE_START:
            game_start();
            break;
        case STATE_GAME:
            game_main();
            break;
        case STATE_CLEAR:
            game_clear();
            break;
        case STATE_MISS:
            game_miss();
            break;
        case STATE_OVER:
            game_over();
            break;
    }

    // vsync処理を許可し、終了を待つ
    vsync_exec = true;
    while (vsync_exec) {
        // NOP
    }
}
