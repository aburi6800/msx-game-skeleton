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

// キャラクターアトリビュートテーブル
CharacterAttr_t characterAttrTbl[32];

// 方向テーブル
static const uint8_t directionTbl[] = {
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

// 象限ごとの DP増分値
static const uint8_t DP_TAB[4] = {
    13, /* 00: X1>X2 , Y1>Y2 */
     9, /* 01: X1>X2 , Y1<=Y2 */
     1, /* 10: X1<=X2, Y1>Y2 */
     5  /* 11: X1<=X2, Y1<=Y2 */
};

// XX を X差で取るか Y差で取るか
static const uint8_t XX_SRC[4] = {
    0, /* 00: X差 */
    1, /* 01: Y差 */
    1, /* 10: Y差 */
    0  /* 11: X差 */
};

// YY を X差で取るか Y差で取るか
static const uint8_t YY_SRC[4] = {
    1, /* 00: Y差 */
    0, /* 01: X差 */
    0, /* 10: X差 */
    1  /* 11: Y差 */
};

// 移動量テーブル
int vx[] = { 0.00,  0.00,  0.38,  0.75,  0.92,  1.00,  0.92,  0.75,  0.38,  0.00, -0.38, -0.75, -0.92, -1.00, -0.92, -0.75, -0.38 };
int vy[] = { 0.00, -1.00, -0.92, -0.75, -0.38,  0.00,  0.38,  0.75,  0.92,  1.00,  0.92,  0.75,  0.38,  0.00, -0.38, -0.75, -0.92 };


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
    vwrite(SPRITE_PATTERN, VRAM_SPR_PTN_TBL, 32 * 2);

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
/* Cの実装
    // VSYNC処理が許可されていないときはreturn
    if (vsync_count) {
        --vsync_count;
        return;
    }
    if (!vsync_exec) {
        return;
    }
    // スプライトアトリビュートテーブル更新
    vwrite(spriteAttrTbl, VRAM_SPR_ATTR_TBL, VRAM_SPR_ATTR_SIZE);
    // パターンネームテーブル更新
    vwrite(screenBuffer, VRAM_PTN_NAME_TBL, VRAM_PTN_NAME_SIZE);
    // VSYNC処理不許可にする
    vsync_exec = false;
    vsync_count = FRAME_RATE;
*/
// clang-format off
#ifndef __INTELLISENSE__
#asm
    // VSYNC処理が許可されていないときはreturn
    LD      A, (_vsync_count)
    OR      A
    JR      Z, _draw_game_1

    DEC     A
    LD      (_vsync_count), A
    RET

_draw_game_1:
    LD      A, (_vsync_exec)
    OR      A
    RET     Z

_draw_game_2:
    // 周辺色変更
;    LD      A, 0x07                     ; COLOR CODE
;    LD      (0xF3EB), A                 ; 0xF3EB = BDRCLR
;    CALL    0x0062                      ; BIOS : CHGCLR

    ; スプライトアトリビュートテーブル更新
    LD      HL, _spriteAttrTbl          ; Source addr
    LD      DE, VRAM_SPR_ATTR_TBL       ; Dist addr
    LD      BC, VRAM_SPR_ATTR_SIZE      ; Length
    CALL    0x005C                      ; BIOS : LDIRVM

    ; パターンネームテーブル更新
    LD      HL, _screenBuffer           ; Source addr
    LD      DE, VRAM_PTN_NAME_TBL       ; Dist addr
    LD      BC, VRAM_PTN_NAME_SIZE      ; Length
    CALL    0x005C                      ; BIOS : LDIRVM

    ; VSYNC処理を不許可にする
    XOR     A
    LD      (_vsync_exec), A
    LD      A, FRAME_RATE
    LD      (_vsync_count), A

    ; 周辺色戻す
;    LD      A, 0x01             ; COLOR CODE
;    LD      (0xF3EB), A         ; 0xF3EB = BDRCLR
;    CALL    0x0062              ; BIOS : CHGCLR
#endasm;
#endif
// clang-format on
}

/*
 * 方向取得処理
 * 方向は上から右周りに1～16で返される
 *
 * args:
 * - ex             uint8_t     開始点のX座標
 * - ey             uint8_t     開始点のY座標
 * - px             uint8_t     目標点のX座標
 * - py             uint8_t     目標点のY座標
 *
 * return:
 * - uint8_t        方向値(1～16)
 */
uint8_t get_direction(uint8_t ex, uint8_t ey, uint8_t px, uint8_t py)
{
    uint8_t x1, y1, x2, y2;
    uint8_t dx, dy;
    uint8_t xx, yy;
    uint8_t q;
    uint8_t dir;

    x1 = (ex * 8) / 16;
    y1 = (ey * 8) / 16;
    x2 = (px * 8) / 16;
    y2 = (py * 8) / 16;

    dx = x2 - x1;
    dy = y2 - y1;

    /* 象限 index（bit1=X, bit0=Y） */
    q = ((dx >= 0) << 1) | (dy >= 0);

    /* 絶対値（branchless でも可） */
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;

    xx = XX_SRC[q] ? dy : dx;
    yy = YY_SRC[q] ? dy : dx;

    dir = directionTbl[(yy << 4) + xx] + DP_TAB[q];
    if (dir > 16) dir -= 16;

    return dir;
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
        characterAttrTbl[i].attr.y = 192;
        characterAttrTbl[i].attr.x = 0;
        characterAttrTbl[i].attr.col = 0;
        characterAttrTbl[i].attr.ptn = 0;
        characterAttrTbl[i].plane = i;
    }

    characterAttrTbl[0].attr.x = 120;
    characterAttrTbl[0].attr.y = 88;
    characterAttrTbl[0].attr.col = 10;
    characterAttrTbl[0].attr.ptn = 0;
    characterAttrTbl[0].vx = 0;
    characterAttrTbl[0].vy = 0;

    characterAttrTbl[1].attr.x = 120;
    characterAttrTbl[1].attr.y = 88;
    characterAttrTbl[1].attr.col = 15;
    characterAttrTbl[1].attr.ptn = 4;
    characterAttrTbl[1].vx = 0;
    characterAttrTbl[1].vy = 0;

    characterAttrTbl[2].attr.x = 48;
    characterAttrTbl[2].attr.y = 80;
    characterAttrTbl[2].attr.col = 13;
    characterAttrTbl[2].attr.ptn = 0;
    characterAttrTbl[2].vx = 0;
    characterAttrTbl[2].vy = -2;

    characterAttrTbl[3].attr.x = 72;
    characterAttrTbl[3].attr.y = 82;
    characterAttrTbl[3].attr.col = 12;
    characterAttrTbl[3].attr.ptn = 0;
    characterAttrTbl[3].vx = 0;
    characterAttrTbl[3].vy = -1;

    characterAttrTbl[4].attr.x = 96;
    characterAttrTbl[4].attr.y = 84;
    characterAttrTbl[4].attr.col = 10;
    characterAttrTbl[4].attr.ptn = 0;
    characterAttrTbl[4].vx = 0;
    characterAttrTbl[4].vy = 2;

    characterAttrTbl[5].attr.x = 144;
    characterAttrTbl[5].attr.y = 90;
    characterAttrTbl[5].attr.col = 7;
    characterAttrTbl[5].attr.ptn = 0;
    characterAttrTbl[5].vx = 0;
    characterAttrTbl[5].vy = -2;

    characterAttrTbl[6].attr.x = 168;
    characterAttrTbl[6].attr.y = 92;
    characterAttrTbl[6].attr.col = 4;
    characterAttrTbl[6].attr.ptn = 0;
    characterAttrTbl[6].vx = 0;
    characterAttrTbl[6].vy = -1;

    characterAttrTbl[7].attr.x = 192;
    characterAttrTbl[7].attr.y = 94;
    characterAttrTbl[7].attr.col = 3;
    characterAttrTbl[7].attr.ptn = 0;
    characterAttrTbl[7].vx = 0;
    characterAttrTbl[7].vy = 2;

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
    for (uint8_t i = 0; i < 32; i++) {
        if (characterAttrTbl[i].vy != 0) {
            characterAttrTbl[i].attr.y += characterAttrTbl[i].vy;
            if (characterAttrTbl[i].attr.y < 8) {
                characterAttrTbl[i].attr.y = 8;
                characterAttrTbl[i].vy = characterAttrTbl[i].vy * -1;
            }
            if (characterAttrTbl[i].attr.y > 168) {
                characterAttrTbl[i].attr.y = 168;
                characterAttrTbl[i].vy = characterAttrTbl[i].vy * -1;
            }
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
