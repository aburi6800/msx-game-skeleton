// License:MIT License
// copyright-holders:Hitoshi Iwai

#ifndef CONST_H
#define CONST_H

// VRAM操作用定数
#define VRAM_PTN_GENR_TBL1  0x0000  // パターンジェネレータテーブル(ページ1)
#define VRAM_PTN_GENR_TBL2  0x0800  // パターンジェネレータテーブル(ページ2)
#define VRAM_PTN_GENR_TBL3  0x1000  // パターンジェネレータテーブル(ページ3)
#define VRAM_PTN_GENR_SIZE  0x0800  // パターンジェネレータテーブルサイズ

#define VRAM_COLOR_TBL1     0x2000  // カラーテーブル(ページ1)
#define VRAM_COLOR_TBL2     0x2800  // カラーテーブル(ページ2)
#define VRAM_COLOR_TBL3     0x3000  // カラーテーブル(ページ3)
#define VRAM_COLOR_SIZE     0x0800  // カラーテーブルサイズ

#define VRAM_PTN_NAME_TBL   0x1800  // パターンネームテーブル
#define VRAM_PTN_NAME_SIZE  768     // パターンネームテーブルサイズ

#define VRAM_SPR_PTN_TBL    0x3800  // スプライトパターンテーブル
#define VRAM_SPR_ATTR_TBL   0x1B00  // スプライトアトリビュートテーブル
#define VRAM_SPR_ATTR_SIZE  128     // スプライトアトリビュートテーブルサイズ

#define SCREEN_WIDTH        32      // 画面幅
#define SCREEN_HEIGHT       24      // 画面高さ

// MSX BIOS
#define MSX_H_TIMI          0xfd9f  // H.TIMI割り込みフックアドレス
#define MSX_CLIKSW          0xf3db  // キークリックスイッチ

// STICK定数
#define STICK_NONE          0
#define STICK_UP            1
#define STICK_UPRIGHT       2
#define STICK_RIGHT         3
#define STICK_DOWNRIGHT     4
#define STICK_DOWN          5
#define STICK_LEFTDOWN      6
#define STICK_LEFT          7
#define STICK_UPLEFT        8

// TRIGGER定数
#define TRIGGER_ON          255
#define TRIGGER_OFF         0

#endif
