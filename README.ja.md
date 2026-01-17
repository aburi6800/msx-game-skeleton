[ [Engligh](README.md) | [日本語](README.ja.md) ]

---

# MSX ゲームプログラム テンプレート

</br>

## 概要

- MSXで動作するゲームプログラムを作成するためのテンプレートプロジェクトです。
- z88dkを使用してコンパイル・romファイルを作成します。
- game.hのGameStatus_tに各ゲームで必要な状態を書き、その他必要な構造体を定義します。
- game.cにゲームの各処理と、draw_game関数に画面描画処理を記述します。
- draw_game関数を呼び出すタイミングは、const.hのFRAME_RATEで指定します。(0=1/60、1=1/30、2=1/15)

</br>

## サポート機能

- 以下の機能がサポートされています。
  - スクリーンバッファへの描画
  - H.TIMI割り込みでの画面描画
  - キーボード/ジョイスティックでの入力情報取得
  - スプライトプレーンシャッフル
  - 2オブジェクト間の16方向の方向検出

</br>

## 未サポート機能

- 以下の機能が未サポートです。
  - サウンドドライバ
  - キーボードのBボタン入力
  - スクロール処理

</br>

## 改訂履歴

ver.20260117

- プロトタイプバージョン

</br>

## ライセンス

MIT License

</br>

## Thanks

- [Z88DK - The Development Kit for Z80 Computers](https://github.com/z88dk/z88dk)
- [C-BIOS](http://cbios.sourceforge.net/)
- [openMSX](https://openmsx.org/)
