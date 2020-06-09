
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
If FM TOWNS ROM image files are stored in ```E:/ROM_MX``` directory, boot from disc image ```E:/ISOImage/AfterburnerII/AFTER_BURNER.CUE```.  Game pad 0 is emulated by keyboard (A,S,Z,X,↑↓←→), and Window size is scaled up to 160%.

FM TOWNSのROMイメージが```E:/ROM_MX```のとき、ディスクイメージ```E:/ISOImage/AFTER_BURNER.CUE```から起動します。ジョイパッド0はキーボードエミュレーション (A,S,Z,X,↑↓←→)、ウィンドウは160%にスケールアップされます。

```
./Tsugaru_CUI E:/ROM_MX -CD E:/ISOImage/AUGUSTA.CUE -FD0 E:/ISOImage/AUGUSTAFD.bin -SCALE 160
```
IF FM TOWNS ROM image files are in ```E:/ROM_MX``` directory, boot from disc image ```E:/ISOImage/AUGUSTA.CUE``` and floppy-disk image ```E:/ISOImage/AUGUSTAFD.bin``` in drive A.  Window is scaled up to 160%.

FM TOWNSのROMイメージが```E:/ROM_MX```のとき、CDイメージ```E:/ISOImage/AUGUSTA.CUE```とドライブAのフロッピーディスクイメージ```E:/ISOImage/AUGUSTAFD.bin```から起動します。ウィンドウは160%にスケールアップされます。

```
./Tsugaru_CUI E:/ROM_MX -CMOS ./CMOS.BIN -CD E:/ISOImage/TownsOSV2.1L20.cue
```
If FM TOWNS ROM image files are stored in E:/ROM_MX, and CMOS file (BIOS Settings in the DOS terminology) is ./CMOS.BIN, boot from disc image ```E:/ISOImage/TownsOSV2.1L20.cue```  If ```./CMOS.BIN``` does not exist, it is created when the program is closed by Q (Quit) command.  If the program is forced to close by window-close button, the file won't be created.

FM TOWNSのROMイメージが```E:/ROM_MX```で、CMOSファイル(DOSで言うところのBIOS設定)が```./CMOS.BIN```のとき、ディスクイメージ```E:/ISOIMage/TownsOSV2.1L20.cue```からシステムを起動します。```./CMOS.BIN```が存在しない場合、コンソールコマンドのQ (Quit) コマンドで終了したとき、新たに```CMOS.BIN```を作成します。ウィンドウを閉じて強制終了した場合はファイルはできません。



# Command Option Reference
```
-HELP,-H,-?
```
Print Help.

ヘルプの表示。

```
-SCALE X
```
Screen scaling X percent.

ウィンドウサイズをX%に拡大または縮小。

```
-PAUSE
```
Machine state is PAUSE on start up.  Type "RUN" command in the console to start the VM.  Use "PAU" command in the console to pause again.  Pressing NumLock key also pauses the VM.

一時停止状態でプログラムを開始します。コンソールで”RUN"コマンドをタイプすることで実行を開始します。コンソールで"PAU"コマンドをタイプすることで再度一時停止することができます。またはNumLockキーを押すことでもVMを一時停止することができます。

```
-DEBUG,-DEBUGGER
```
Start the machine with debugger enabled.  To disable the debugger in the VM, type "DIS DEBUG" command, or to re-enable, type "ENA DEBUG" in the console.  VM execution will be slower when the debugger is enabled.

デバッガを有効にした状態で起動します。デバッガを無効にするにはコンソールで"DIS DEBUG"とタイプします。再度有効にするには"ENA DEBUG"とタイプします。デバッガが有効な状態ではVMの実行はやや遅くなります。

```
-UNITTEST
```
Let it run automatically to the end without taking control commands.

VMの電源が切れたとき、コマンドを待たずにプログラムを終了します。

```
-FREQ frequency_in_MHz
```
Specify CPU frequency in Megahertz.  Default frequency is 25MHz.  Setting faster clock speed does not necessarily speed up the execution.  With 3GHz Core i7 host, the execution speed increases up to 20MHz to 25MHz, but beyond that the CPU time laggs behind the real time, which will lag the response from the devices.

CPUのクロック周波数を指定します。ホストのCPUが3GHz Core i7の場合、25MHｚ程度までは大きな数字を指定する方がスピードが上がりますが、それ以上の値を指定するとCPU時間が実時間より遅れはじめるため、デバイスの反応も遅くなり、結果的にVMの実行がかえって遅くまります。

```
-NOWAIT
```
VM never waits for real time when VM time runs ahead of the real time.

実時間調整をせずにVMを実行します。

```
-YESWAIT
```
VM always waits for real time when VM time runs ahead of the real time.

実時間調整を有効にします。

```
-NOWAITBOOT
```
No wait during the memory test, and then VM waits for real time when VM time runs ahead of the real time.

電源投入後最初のディスクアクセス前(メモリテスト等)の間だけ実時間調整を無効にして最大速度で実行して、最初のディスクアクセス以後実時間調整を有効にします。


```
-CMOS filename
```
Specify CMOS (BIOS settings) file.  If you specify this option, CMOS settings will be saved automatically when closing the program unless -DONTAUTOSAVECMOS option is also specified.  If CMOS file does not exist, the program will auto-create a file.

CMOS(DOSで言うところのBIOS設定)を保存するファイルを指定します。このオプションを指定した場合、VM終了時にCMOS設定の内容を指定のファイルに保存します。(ただし-DONTAUTOSAVECMOSオプションを一緒に指定した場合を除く)。もしもCMOSファイルが存在しない場合、新しいファイルを作成します。


```
-DONTAUTOSAVECMOS
```
This option will prevent VM from saving CMOS file on exit.

VM終了時のCMOSファイルの生成/上書きを無効にします。

```
-FD0 filename
```
Floppy disk image file name for Drive A.

ドライブ0のフロッピーディスクイメージファイルを指定します。

```
-FD1 filename
```
Floppy disk image file name for Drive B.

ドライブ1のフロッピーディスクイメージファイルを指定します。

```
-CD image-file-name
```
CD-ROM image file name for the internal drive. ISO or CUE.

内蔵CD-ROMドライブのCDイメージファイルを指定します。ISOまたはCUEファイルを指定します。

```
-GAMEPORT0 KEY|PHYS|NONE
-GAMEPORT1 KEY|PHYS|NONE
```
Specify game-port emulation.  By keyboard (↑↓←→,Z,X,A,S), or physical gamepad.  (I haven't implemented physical gamepad yet.  It's planned.)

ジョイパッドのエミュレーションを指定します。KEYを指定するとキーボードでエミュレート(↑↓←→,Z,X,A,S)、またはPHYSを指定するとPCに接続されてジョイスティックを使います。(なお、PC接続のジョイスティックはまだ実装してないので予定)

```
-HD0 image-file-name
```
SCSI Hard-disk image file name.  Can be -HDx (0<=x<=6)  Hard-disk image is compatible with the image file of FM Towns emulator UNZ.

SCSI接続のハードディスクイメージファイルを指定します。UNZのイメージファイルと互換性があります。

```
-SCSICD0 image-file-name
```
SCSI CD-ROM image file name.  Can be -SCSICDx (0<=x<=6)   (Planned)

SCSI接続のCD-ROMイメージを指定します(予定)

```
-SYM filename
```
Specify symbol file name.

デバッガで使うシンボル情報を記憶するファイルを指定します。

```
-EVTLOG filename
```
Load and play-back event log. (For unit tests)

再生用のイベントログファイルを指定します。(ユニットテスト用)

```
-HOST2VM hostFileName vmFileName
```
Schedule Host to VM file transfer.  File will be transferred when FTCLIENT.EXP is running.

ホストからVMのファイル転送を指定します。VM内で転送プログラムFTCLIENT.EXPを起動する必要があります。

```
-PRETEND386DX
```
With this option, VM will report CPU as 386DX from I/O port 0030H.  DOS-Extender that comes with Towns OS V2.1L10 checks the CPU and blocks if the CPU is not 386DX nor 386SX.  That is probably why we see many software titles running on Towns OS V2.1L10B, but not with V2.1L10.  This option is to start Towns OS V2.1L10-based titles such as Free Software Collection 4 Disc B.

このオプションを指定すると、VMのCPU識別情報(I/O 0030H)を386DX扱いにします。Towns OS V2.1L10にはL10無印およびL10Bのふたつのバージョンがあります。無印のV2.1L10のDOS-Extenderは、CPU式別情報が386DXか386SXでないと実行できなくなっています。そのような場合、このオプションを指定することで起動できます。

```
-APP title
```
Use application-specific customization.  You can specify one of the following as title
- ```WC1``` or ```WINGCOMMANDER1```  Enable special mouse integration for Wing Commander.  By the way, press ```Z``` key if your fighter spins crazy when the mission starts.

プログラムに特化したカスタマイズを有効にします。titleに指定可能なのアプリケーションは次の通りです。
- ```WC1``` or ```WINGCOMMANDER1```  Wing Commander用のマウスインテグレーションを有効にします。なお、ミッション開始時に機体が制御不能に回転を始めたら```Z```キーを押すと安定して操縦できるようになります。

```
-GENFD filename.bin size_in_KB
```
Create a new floppy image.  size_in_KB must be 1232, 1440, 640, or 720.

新しいフロッピーディスクイメージを作成します。サイズはキロバイトで指定で、1232, 1440, 640, または 720 のひとつを指定します。

```
-GENHD filename.bin size_in_MB
```
Create a new harddisk image.

新しいハードディスクイメージを作成します。




# VM Command Reference

```Tsugaru_CUI```実行中のフロッピーディスクイメージやCDイメージの交換が必要な場合などの操作はコンソール上のコマンドを使います。

In ```Tsugaru_CUI``` you need to type a command on the console to control the VM such as changing CD and FD images.


