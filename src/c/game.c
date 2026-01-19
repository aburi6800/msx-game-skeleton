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
uint8_t enemy_bullet_num = 0;

// キャラクターアトリビュートテーブル
CharacterAttr_t characterAttrTbl[32];

// 敵弾パターンテーブル
uint8_t enemy_bullet_ptn[2] = {8, 12};


// 文字列定義
uint8_t TITLE1[] = "SHOOTING GAME";
uint8_t TITLE2[] = "PUSH START BUTTON";
uint8_t TITLE3[] = "(C)ABURI GAMES 2026";
uint8_t TITLE4[] = "ALL RIGHT RESERVED";

// スコアデータ (BCD値)
unsigned char score[3] = {0x00, 0x00, 0x00};

// プレイヤー移動テーブル
uint8_t playerMoveTbl[9] = { 0,  1,  3,  5,  7,  9, 11, 13, 15};

// スクロール用のマップデータ
// 縦2画面分を確保する
//uint8_t mapData[MAPDATA_SIZE];

// マップの転送開始位置
//uint16_t mapOffset;


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
    vwrite(SPRITE_PATTERN, VRAM_SPR_PTN_TBL, 32 * 4);

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
 * マップデータ初期化
 * mapDataを引数のキャラクターで埋める
 *
 * args:
 * - data           uint8_t     マップデータを埋めるキャラクターコード
 * 
 * return:
 * - void
 */
void initMapData(uint8_t data)
{
//    for (uint16_t i = 0; i < MAPDATA_SIZE; i++) {
//        mapData[i] = data;
//    }
}


/*
 * マップデータ書き込み
 * mapDataの指定位置にマップデータを書き込む
 *
 * args:
 * - pos            uint16_t    マップデータの書き込み位置
 * - data           uint8_t     マップデータ（キャラクターコード）
 * 
 * return:
 * - void
 */
void writeMapData(uint16_t pos, uint8_t data)
{
//    mapData[pos] = data;    
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
    characterAttrTbl[2].type = TYPE_ENEMY;
    characterAttrTbl[2].attr.x = 48;
    characterAttrTbl[2].attr.y = 80;
    characterAttrTbl[2].attr.col = 13;
    characterAttrTbl[2].attr.ptn = 0;
    characterAttrTbl[2].x = characterAttrTbl[2].attr.x << 8;
    characterAttrTbl[2].y = characterAttrTbl[2].attr.y << 8;
    characterAttrTbl[2].vx = 0;
    characterAttrTbl[2].vy = -2;
    characterAttrTbl[2].count = 10;

    characterAttrTbl[3].type = TYPE_ENEMY;
    characterAttrTbl[3].attr.x = 72;
    characterAttrTbl[3].attr.y = 82;
    characterAttrTbl[3].attr.col = 12;
    characterAttrTbl[3].attr.ptn = 0;
    characterAttrTbl[3].x = characterAttrTbl[3].attr.x << 8;
    characterAttrTbl[3].y = characterAttrTbl[3].attr.y << 8;
    characterAttrTbl[3].vx = 0;
    characterAttrTbl[3].vy = -1;
    characterAttrTbl[3].count = 15;

    characterAttrTbl[4].type = TYPE_ENEMY;
    characterAttrTbl[4].attr.x = 96;
    characterAttrTbl[4].attr.y = 84;
    characterAttrTbl[4].attr.col = 10;
    characterAttrTbl[4].attr.ptn = 0;
    characterAttrTbl[4].x = characterAttrTbl[4].attr.x << 8;
    characterAttrTbl[4].y = characterAttrTbl[4].attr.y << 8;
    characterAttrTbl[4].vx = 0;
    characterAttrTbl[4].vy = 2;
    characterAttrTbl[4].count = 22;

    characterAttrTbl[5].type = TYPE_ENEMY;
    characterAttrTbl[5].attr.x = 144;
    characterAttrTbl[5].attr.y = 90;
    characterAttrTbl[5].attr.col = 7;
    characterAttrTbl[5].attr.ptn = 0;
    characterAttrTbl[5].x = characterAttrTbl[5].attr.x << 8;
    characterAttrTbl[5].y = characterAttrTbl[5].attr.y << 8;
    characterAttrTbl[5].vx = 0;
    characterAttrTbl[5].vy = -2;
    characterAttrTbl[5].count = 29;

    characterAttrTbl[6].type = TYPE_ENEMY;
    characterAttrTbl[6].attr.x = 168;
    characterAttrTbl[6].attr.y = 92;
    characterAttrTbl[6].attr.col = 4;
    characterAttrTbl[6].attr.ptn = 0;
    characterAttrTbl[6].x = characterAttrTbl[6].attr.x << 8;
    characterAttrTbl[6].y = characterAttrTbl[6].attr.y << 8;
    characterAttrTbl[6].vx = 0;
    characterAttrTbl[6].vy = -1;
    characterAttrTbl[6].count = 36;

    characterAttrTbl[7].type = TYPE_ENEMY;
    characterAttrTbl[7].attr.x = 192;
    characterAttrTbl[7].attr.y = 94;
    characterAttrTbl[7].attr.col = 3;
    characterAttrTbl[7].attr.ptn = 0;
    characterAttrTbl[7].x = characterAttrTbl[7].attr.x << 8;
    characterAttrTbl[7].y = characterAttrTbl[7].attr.y << 8;
    characterAttrTbl[7].vx = 0;
    characterAttrTbl[7].vy = 2;
    characterAttrTbl[7].count = 41;

/*
    // マップデータ初期化
    initMapData(0x20);

    // マップデータ作成
    writeMapData( 8 * 32 +  6, 'T');
    writeMapData( 8 * 32 +  7, 'E');
    writeMapData( 8 * 32 +  8, 'S');
    writeMapData( 8 * 32 +  9, 'T');
    writeMapData( 8 * 32 + 10, 'M');
    writeMapData( 8 * 32 + 11, 'A');
    writeMapData( 8 * 32 + 12, 'P');
    writeMapData(16 * 32 + 18, 'T');
    writeMapData(16 * 32 + 19, 'E');
    writeMapData(16 * 32 + 20, 'S');
    writeMapData(16 * 32 + 21, 'T');
    writeMapData(16 * 32 + 22, 'M');
    writeMapData(16 * 32 + 23, 'A');
    writeMapData(16 * 32 + 24, 'P');
    writeMapData(11 * 32 + 16, '2');

    writeMapData(32 * 32 +  6, 'T');
    writeMapData(32 * 32 +  7, 'E');
    writeMapData(32 * 32 +  8, 'S');
    writeMapData(32 * 32 +  9, 'T');
    writeMapData(32 * 32 + 10, 'M');
    writeMapData(32 * 32 + 11, 'A');
    writeMapData(32 * 32 + 12, 'P');
    writeMapData(40 * 32 + 18, 'T');
    writeMapData(40 * 32 + 19, 'E');
    writeMapData(40 * 32 + 20, 'S');
    writeMapData(40 * 32 + 21, 'T');
    writeMapData(40 * 32 + 22, 'M');
    writeMapData(40 * 32 + 23, 'A');
    writeMapData(40 * 32 + 24, 'P');
    writeMapData(36 * 32 + 16, '1');

    // マップのオフセット位置設定
    mapOffset = 24 * 32;
*/

    clear_screenbuffer();

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

        // 敵
        } else if (character->type == TYPE_ENEMY) {
            // 移動
            // 手を抜いてattrのx,yを直接更新しているが、本来はx,yを更新後にattr.x,yを算出すべき
            if (character->vy != 0) {
                character->attr.y += character->vy;
                if (character->attr.y < 8) {
                    character->attr.y = 8;
                    character->vy = character->vy * -1;
                }
                if (character->attr.y > 168) {
                    character->attr.y = 168;
                    character->vy = character->vy * -1;
                }
                // 一応内部座標も入れておく
                character->x = character->attr.x << 8;
                character->y = character->attr.y << 8;
            }
            // 弾を撃つか？
            if (character->count-- == 0) {
                // 次の発射までのカウンタリセット
                character->count = 40;
                // 弾を生成できる？
                if (enemy_bullet_num < ENEMY_BULLET_MAX) {
                    // 弾を生成
                    for (uint8_t i = 0; i < ENEMY_BULLET_MAX; i++) {
                        if (characterAttrTbl[i].type == TYPE_NONE) {
                            characterAttrTbl[i].type = TYPE_E_BULLET;
                            characterAttrTbl[i].attr.x = character->attr.x;
                            characterAttrTbl[i].attr.y = character->attr.y;
                            characterAttrTbl[i].x = character->attr.x << 8;
                            characterAttrTbl[i].y = character->attr.y << 8;
                            characterAttrTbl[i].attr.col  = 7;
                            characterAttrTbl[i].attr.ptn  = enemy_bullet_ptn[game.tick % 2];
                            uint8_t d = get_direction(character->attr.x, character->attr.y, characterAttrTbl[0].attr.x, characterAttrTbl[0].attr.y);
//                            characterAttrTbl[i].speed = 8;
                            characterAttrTbl[i].vx = (vx[d] * 8); // 8 = speed
                            characterAttrTbl[i].vy = (vy[d] * 8); // 8 = speed

                            enemy_bullet_num++;
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

                enemy_bullet_num--;
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
