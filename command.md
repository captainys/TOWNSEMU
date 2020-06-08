
# FM TOWNS Emulator "Tsugaru" Command Reference
# FM TOWNS エミュレータ 「津軽」 コマンドリファレンス
by CaptainYS

<http://www.ysflight.com>

PEB01130@nifty.com



# Command-Line Examples
```
.\Tsugaru_CUI.exe -HELP
```
ヘルプを表示します。

Print help.

```
./Tsugaru_CUI E:/ROM_MX -CD E:/ISOImage/AFTER_BURNER.CUE -GAMEPORT0 KEY -SCALE 160
```
FM TOWNSのROMイメージが```E:/ROM_MX```のとき、ディスクイメージ```E:/ISOImage/AFTER_BURNER.CUE```から起動します。ジョイパッド0はキーボードエミュレーション (A,S,Z,X,↑↓←→)、ウィンドウは160%にスケールアップされます。

If FM TOWNS ROM image files are stored in ```E:/ROM_MX``` directory, boot from disc image ```E:/ISOImage/AfterburnerII/AFTER_BURNER.CUE```.  Game pad 0 is emulated by keyboard (A,S,Z,X,↑↓←→), and Window size is scaled up to 160%.

```
./Tsugaru_CUI E:/ROM_MX -CD E:/ISOImage/AUGUSTA.CUE -FD0 E:/ISOImage/AUGUSTAFD.bin -SCALE 160
```
FM TOWNSのROMイメージが```E:/ROM_MX```のとき、CDイメージ```E:/ISOImage/AUGUSTA.CUE```とドライブAのフロッピーディスクイメージ```E:/ISOImage/AUGUSTAFD.bin```から起動します。ウィンドウは160%にスケールアップされます。

IF FM TOWNS ROM image files are in ```E:/ROM_MX``` directory, boot from disc image ```E:/ISOImage/AUGUSTA.CUE``` and floppy-disk image ```E:/ISOImage/AUGUSTAFD.bin``` in drive A.  Window is scaled up to 160%.

```
./Tsugaru_CUI E:/ROM_MX -CMOS ./CMOS.BIN -CD E:/ISOImage/TownsOSV2.1L20.cue
```
FM TOWNSのROMイメージが```E:/ROM_MX```で、CMOSファイル(DOSで言うところのBIOS設定)が```./CMOS.BIN```のとき、ディスクイメージ```E:/ISOIMage/TownsOSV2.1L20.cue```からシステムを起動します。```./CMOS.BIN```が存在しない場合、コンソールコマンドのQ (Quit) コマンドで終了したとき、新たに```CMOS.BIN```を作成します。ウィンドウを閉じて強制終了した場合はファイルはできません。

If FM TOWNS ROM image files are stored in E:/ROM_MX, and CMOS file (BIOS Settings in the DOS terminology) is ./CMOS.BIN, boot from disc image ```E:/ISOImage/TownsOSV2.1L20.cue```  If ```./CMOS.BIN``` does not exist, it is created when the program is closed by Q (Quit) command.  If the program is forced to close by window-close button, the file won't be created.
