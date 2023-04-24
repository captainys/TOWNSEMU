Windows 3.1 Mouse Integrator

Windows 3.1 L10 does not use hardware mouse cursor, and therefore Tsugaru cannot detect where mouse curosr is.
Windows 3.1 L11 and L12 does use hardware mouse cursor when the mouse cursor is black and white.  However,
when the application uses a color mouse cursor, it falls back to software mouse cursor, and Tsugaru cannot
detect where mouse cursor is.  If Tsugaru cannot detect the cursor position, it cannot calculate the difference
between host cursor position and Windows 3.1 cursor position, and mouse integration does not work.

For this problem, you can copy this TGMOUSE.EXE to somewhere in the Windows 3.1 installation, and register it
in the Start Up group.  This program continuously poll Windows 3.1 mouse position and report it back to Tsugaru
host so that mouse integration works.


Windows 3.1マウスインテグレータ

Windows 3.1 L10はハードウェアマウスカーソルを使わないので、津軽ホスト側ではどこにマウスカーソルがあるのかわか
りません。Windows 3.1 L11, L12はハードウェアマウスカーソルを使うので、津軽ホストはCRTCレジスタからカーソル位置
を把握することができますが、それはカーソルが白黒の間だけで、カラーのマウスカーソルに切り替わるとソフトウェア
カーソルになるため、やっぱり位置がわかりません。Windows 3.1内のカーソル位置がわからないと、ホストカーソル位置
との差分が計算できないため、マウスインテグレーションが動かなくなります。

この問題を解決するために、このTGMOUSE.EXEをWindows 3.1のドライブのどこかにコピーして、スタートアップグループに
アイコン登録してみてください。このプログラムは起動中常にホストにカーソル位置をレポートし続けるのでマウスインテ
グレーションが動くようになります。
