
# FM TOWNS Emulator "Tsugaru" Command Reference
# FM TOWNS エミュレータ 「津軽」 コマンドリファレンス
by CaptainYS

<http://www.ysflight.com>

PEB01130@nifty.com



# Command-Line Examples
```
.\Tsugaru_CUI.exe -HELP
```
Print help.

ヘルプを表示します。

```
./Tsugaru_CUI E:/ROM_MX -CD E:/ISOImage/AFTER_BURNER.CUE -GAMEPORT0 KEY -SCALE 160
```
If FM TOWNS ROM image files are stored in ```E:/ROM_MX``` directory, boot from disc image ```E:/ISOImage/AfterburnerII/AFTER_BURNER.CUE```.  Game pad 0 is emulated by keyboard (A,S,Z,X,↑↓←→), and Window size is scaled up to 160%.

If you have a physical game pad, you can use ```PHYS0```, ```PHYS1```, ```PHYS2```, or ```PHYS3``` in place for ```KEY```.  Or, in case your game pad reports direction button as the analog input, use ```ANA0```,```ANA1```, ```ANA2``` or ```ANA3```.

FM TOWNSのROMイメージが```E:/ROM_MX```のとき、ディスクイメージ```E:/ISOImage/AFTER_BURNER.CUE```から起動します。ジョイパッド0はキーボードエミュレーション (A,S,Z,X,↑↓←→)、ウィンドウは160%にスケールアップされます。

USBやBluetooth接続のゲームパッドがある場合は、```KEY````の代わりに```PHYS0```, ```PHYS1```, ```PHYS2```, または ```PHYS3```を指定すると使えます。また、十字ボタンをハットスイッチではなくアナログレバーとして扱うゲームパッドの場合は、```ANA0```,```ANA1```,```ANA2```,```ANA3```を指定することで利用できます。

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
# オプションリファレンス
This section explains the options that you can specify when you start Tsugaru_CUI executable.

このセクションはTsugaru_CUIを起動するとき指定可能なオプションを説明します。

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
-FD0WP,-FD1WP
```
Write protect floppy disk.

フロッピーディスクイメージを書き込み禁止にします。

```
-FD0UP,-FD1UP
```
Write unprotect floppy disk.

フロッピーディスクイメージを書き込み可にします。

```
-CD image-file-name
```
CD-ROM image file name for the internal drive. ISO or CUE.

内蔵CD-ROMドライブのCDイメージファイルを指定します。ISOまたはCUEファイルを指定します。

```
-GAMEPORT0 KEY|PHYSx|ANAx|NONE
-GAMEPORT1 KEY|PHYSx|ANAx|NONE
```
Specify game-port emulation.  By keyboard (↑↓←→,Z,X,A,S), or physical gamepad.

PHYS0,PHYS1,PHYS2, and PHYS3 use game-pad direction button (or hat switch) as up/down/left/right.

ANA0,ANA1,ANA2, and ANA3 use game-pad analog stick 0 as up/down/left/right.

ジョイパッドのエミュレーションを指定します。KEYを指定するとキーボードでエミュレート(↑↓←→,Z,X,A,S)、またはPHYSを指定するとPCに接続されてジョイスティックを使います。

PHYS0,PHYS1,PHYS2,PHYS3を指定するとゲームパッドの方向ボタン(またはハットスイッチ)を方向ボタンとして使います。

ANA0,ANA1,ANA2,ANA3を指定するとゲームパッドのアナログスティック0を方向ボタンとして使います。

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
# 仮想マシンコマンドリファレンス

```Tsugaru_CUI```実行中のフロッピーディスクイメージやCDイメージの交換が必要な場合などの操作はコンソール上のコマンドを使います。

In ```Tsugaru_CUI``` you need to type a command on the console to control the VM such as changing CD and FD images.


## General Commands
## 一般コマンド

```
HELP
HLP
H
?
```
Print help.

ヘルプを表示します。

```
QUIT
Q
```
Quit.  When the power is turned off in the VM, type this command to close the console window.

津軽を終了します。VM上で電源オフにした場合、コンソールウィンドウ上でこのコマンドを使ってコンソールウィンドウを閉じてください。

```
RUN
```
Run.  Can specify temporary break point.

一時停止状態からVMの実行を開始します。既に実行中の場合は何も変わりません。

```
CDLOAD filename
```
Load CD-ROM image.

内蔵CD-ROMのCDイメージを指定します。

```
CDOPENCLOSE
```
Virtually open and close the internal CD-ROM drive.

内蔵CD-ROMのイメージはそのままで、いったん蓋を開いて閉じたことにします。

```
FD0LOAD filename
FD1LOAD filename
```
Load FD image.  The number 0 or 1 is the drive number.

フロッピーディスクイメージを指定します。0または1でドライブを指定します。

```
FD0EJECT
FD1EJECT
```
Eject FD.

フロッピーディスクを取り出します。

```
FD0WP
FD1WP
```
Write-protect floppy disk.

フロッピーディスクを書き込み禁止にします。

```
FD0UP
FD1UP
```

Write un-protect floppy disk.

フロッピーディスクを書き込み可能にします。


```
PAUSE
PAU
```
Pause VM.

VMを一時停止します。


## Debugger Commands
## デバッガコマンド

Debugger commands are effective when the debugger is enabled either by ```-DEBUG``` option or by typing ```ENA DEBUG``` command, except some commands that are always available.

デバッガコマンドは、一部のコマンドを除いてデバッガが有効のときのみ使用可能です。デバッガを有効にするには起動時に```-DEBUG```オプションを指定するか、あるいは```ENA DEBUG```コマンドをタイプしてください。



```
RUN EIP
RUN CS:EIP
```
Run to the temporary break point.  Address should be specified as a hexadecimal number.  If you omit the segment, it runs to the offset in the current code segment.

一時的にブレークポイントを設定してそのブレークポイントまで実行します。アドレスは16進数で指定します。セグメントの値を省略した場合は現在のCSの指定オフセットを一時ブレークポイントとします。


```
T
```
Trace.  Run one instruction.

1インストラクション実行します。


```
RET
RTS
```
Run until return from the current procedure.  Available only when call-stack is enabled.  Call-stack is by default enabled when the debugger is enabled.

現在実行中のプロシージャから戻るまで実行します。Call-Stackを有効にしておく必要があります。Call-Stackはデバッガを有効にすると同時に有効になります。

```
INTERRUPT INTNum
```
Inject interrupt.  Same as CPU Instruction INT INTNum.  INTNum is hexadecimal.    For example, INTERRUPT 4B will work same as INT 4BH.

強制的に割り込みを発生させます。INTNumは16進数で指定します。例えば、割り込み4BH(VSYNC)を発声させるには、```INTERRUPT 4B```とタイプします。

```
ADTR SEG:OFFSET
```
Translate address to linear address and physical address.  Address needs to be specified by the hexadecimal number.  To break the VM on memory read/write you can use ```BRKON MEMR``` and ```BRKON MEMW``` commands, which take a physical address.  This ```ADTR``` command is useful for getting the target physical address.

アドレスを線形アドレス、物理アドレスに変換します。```BRKON MEMR```, ```BRKON MEMW```を使うことで特定アドレスへのメモリ読み書きでVMをブレークすることができますが、両コマンドとも物理アドレス指定が必要です。```ADTR```コマンドを使って目的の箇所の物理アドレスを計算することができます。

```
U SEG:Offset
```
Unassemble (disassemble)  If you omit the address, it starts disassembly from the last instruction pointer, then last disassembly location.

指定アドレスから逆アセンブルします。アドレスを省略した場合は、最後に実行した箇所から、次に最後に逆アセンブルした箇所から逆アセンブルします。

```
U16 SEG:Offset
```
Unassemble (disassemble) as 16-bit operand size

16ビットオペランドサイズで逆アセンブルします。

```
U32 SEG:Offset
```
Unassemble (disassemble) as 32-bit operand size

32ビットオペランドサイズで逆アセンブルします。

```
ADDSYM SEG:OFFSET label
```
Add a symbol.  An address can have one symbol,label,data label, or data, and one comment.  If a symbol is added to an address that already has a symbol, label, or data label, the address's label, or data label will be overwritten as a symbol.  Symbols will be shown in disasembly.  If you specify -SYM option when starting the program, symbol information will be automatically saved in the specified.

指定アドレスにシンボルを追加します。追加したシンボルは逆アセンブル時に表示されます。起動時に-SYMオプションを追加した場合は、シンボル情報は指定のファイルに自動的に保存されます。

```
ADDLAB SEG:OFFSET label
ADDLABEL SEG:OFFSET label
```
Add a label.  An address can have one symbol,label, or data label, or data and one comment.  If a symbol is added to an address that already has a symbol, label, or data label, the address's label, or data label will be overwritten as a label.  If you specify -SYM option when starting the program, symbol information will be automatically saved in the specified.

指定アドレスにラベルを追加します。追加したラベルは逆アセンブル時に表示されます。起動時に-SYMオプションを追加した場合は、シンボル情報は指定のファイルに自動的に保存されます。

```
ADDDLB SEG:OFFSET label
ADDDATALABEL SEG:OFFSET label
```
Add a data label.  An address can have one symbol,label, or data label, or data and one comment.  If a symbol is added to an address that already has a symbol, label, or data label, the address's label, or data label will be overwritten as a data label.

指定アドレスにデータラベルを追加します。



```
ADDREM SEG:OFFSET label
ADDCMT SEG:OFFSET label
```
Add a comment.  An address can have one symbol,label, or data label, and one comment.

コメントを追加します。追加したコメントは逆アセンブル時に表示されます。

```
DEFRAW SEG:OFFSET label numBytes
```
Define raw data bytes.  Disassembler will take this address as raw data.  For example, when jump table is written in CS, defining raw bytes can prevent disassembly break down.

指定アドレスから指定バイト数をデータとして扱います。例えば、CSにジャンプテーブルが書いてある場合など、ジャンプテーブル付近で逆アセンブルが崩れることを防止することができます。

```
IMMISIO SEG:OFFSET
```
Take Imm operand of the address as IO-port address.  If the disassembly at the specified address is ```MOV DX,0???H```, and if the I/O address is has a known functionality, disassembly will show I/O label as comment.

指定アドレスのインストラクションのImmオペランドをIOアドレスとして逆アセンブルします。例えば、指定アドレスの逆アセンブルが```MOV DX,0???H```で、I/Oアドレスの機能がわかっている場合、逆アセンブルのコメントにI/Oアドレスのラベルを表示します。


```
DELSYM SEG:OFFSET label
```
Delete a symbol, label, comment, etc. assigned at the address.  A symbol and comment associated with the address will be deleted.

指定アドレスに割り当てられたシンボル、ラベル、コメント等を削除します。

```
WAIT
```
Wait until VM becomes PAUSE state by reaching a break point or break condition.

ブレークポイントやブレークコンディションによってVMが一時停止するまで待ちます。

```
ENA feature
ENABLE feature
```
Enable a feature.  See below for the features that can be enabled or disabled.

機能を有効化します。  有効化・無効化できる機能に関しては下の章を参照してください。ところでMark Downってドキュメント内にリンク張れないのか。というか、リンク張る方法というの試したけどリンクにならないし。全然だめだね。

```
DIS feature
DISABLE feature
```
Disable a feature.

機能を無効化します。

```
PRINT info
PRI info
P info
DUMP info
DM info
```
Print/Dump information.  See below for the information that can be printed.

情報を表示します。表示可能な情報については以下の章を参照してください。

```
CALC formula
```
Caluclate a formula.  Other commands takes hexadecimal numbers, but in this command, you need to write like 0xABCD or ABCDH to describe a hexadecimal number.  Otherwise the number is taken as decimal number.

値を計算します。これ以外のコマンドについて、数値は原則として16進数で指定しますが、この機能では16進数は0xABCDまたはABCDHのように書きます。0xまたはHがないばあいは10進数として扱います。

```
BP EIP
BRK EIP
BP CS:EIP
BRK CS:EIP
```
Add a break point.

ブレークポイントを追加します。

```
BC Num
```
Delete a break point.  ```Num``` is the number printed by ```BL```.  ```BC *``` to erase all break points.

ブレークポイントを削除します。```Num```には```BL```コマンドで表示される番号を指定します。```BC *```とタイプするとすべてのブレークポイントを削除します。

```
BL
```
List break points.

ブレークポイントのリストを表示します。

```
BRKON event
```
Break on event.  See below section for the events that can break.

ブレークするイベントを指定します。指定可能なイベントについては以下の章を参照してください。

```
CBRKON event
```
Clear break-on event.

ブレークイベントを削除します。

```
TYPE characters
```
Send keyboard codes.

キーコードを送信します。

```
KEYBOARD keyboardMode
```
Select ```TRANSLATE``` or ```DIRECT``` mode.  ```TRANSLATE``` or ```TRANS``` mode translate a typed character to FM Towns keycode, therefore you can type with US keyboard or Japanese keyboard.  It is good for typing commands, but cannot sense key release correctly.  ```DIRECT``` mode is good for games, but affected by the keyboard layout.  US keyboard cannot type some of the characters if you are using non-Japanese keyboard.

```TRANSLATE```モードまたは```DIRECT```モードを選びます。```TRANSLATE```モード(```TRANS```と省略も可)では、キーボードが日本語キーボードだろうが英語キーボードだろうがタイプした文字をFM Townsのキーコードに変換して送信するので基本的に使ってるキーボード通りにタイプできます。が、FM-7のようにキーリリースを検出できない、というか1文字タイプしたらリリースしたことになってしまうのでゲームなどには向きません。```DIRECT```モードでは文字の変換をしない代わりにキーリリースを正しく送信します。が、英語キーボードだとタイプできない文字があります。日本語キーボードも試してないからタイプできないキーがあるかも。


```
LET register value
```
Load a register value.

レジスタの値を変更します。

```
CRTCPAGE 1|0 1|0
```
Turn on/off display page.

ディスプレイの表示ページをそれぞれオン・オフします。

```
CMOSLOAD filename
```
Load CMOS.

CMOSデータを読み込みます。

```
CMOSSAVE filename
```
Save CMOS.

CMOSデータを保存します。

```
SAVEHIST filename.txt
```
Save CS:EIP Log to file.  Tsugaru records last 65536 steps of CS:EIP.

「津軽」は過去65536ステップのCS:EIPを記録しています。その記録をファイルに保存します。

```
SAVEEVT filename.txt
```
Save Event Log to file.  I made it for  unit testing.  Probably not very useful for general purposes.

イベントログを保存します。ユニットテスト作成用に作ったので一般用とにはあまり役に立たないと思います。

```
HOST2VM hostFileName vmFileName
```
Schedule Host to VM file transfer.  File will be transferred when FTCLIENT.EXP is running.

ホストからVMへのファイル転送をスケジュールします。FTCLIENT.EXPを実行したときにファイルが転送されます。



## Features that can be enabled/disabled
## 有効化/無効化できる機能
```
CMDLOG
```
Command log.  Saved to CMD.LOG.

コマンドログ。CMD.LOGに保存されます。

```
AUTODISASM
```
Disassemble while running.

逆アセンブルを表示しながら実行します。

```
IOMON iopotMin ioportMax
```
IO Monitor.  ioportMin and ioportMax are optional.  Can specify multiple range by enabling IOMON multiple times.

IOモニタ。ioportMin, ioportMaxを省略するとすべてのI/Oアクセスをモニタします。

```
EVENTLOG
```
Event Log.

イベントログ。

```
DEBUGGER
DEBUG
```
Debugger.

デバッガ。

```
MOUSEINTEG
```
Mouse Integration.

マウスインテグレーション。

## Information that can be printed
## プリントできる情報
```
CALLSTACK|CST
```
Call Stack

コールスタック。

```
SYM
```
Symbol table

シンボルテーブル。


```
HIST
```
Log of CS:EIP.  Can specify number of steps.  Same as HIST command.

直近のインストラクションポインタのヒストリ。


```
GDT
```
Protected-Mode Global Descriptor Table

```
LDT
```
Protected-Mode Local Descriptor Table

```
IDT
```
Protected-Mode Interrupt Descriptor Table

```
RIDT
```
Real-mode Interrupt Descriptor Tables

```
BREAKPOINT
BRK
```
Break Points

```
STATUS
STATE
STA
S
```
Current status.  Same as STA command.

CPUステータス。

```
PIC
```
Pilot-In-Command. No!  Programmable Interrupt Controller.

```
DMA
DMAC
```
DMA Controller.

```
FDC
```
Floppy Disk Controller.

```
SCSI
```
SCSI Controller.

```
CRTC
```
CRTC.

```
PALETTE
```
Palette.

```
TIMER
```
Interval Timer (i8253)

```
MEM
```
Memory Settings

```
CMOS addr
```
CMOS RAM

```
CDROM
```
CD-ROM Status.

```
SCHED
```
Device call-back schedule.

```
SPRITE
```
Sprite status.

## Event that can break
The parameters that can be given to ```BRKON``` and ```CBRKON``` commands.  When the event is detected, the VM breaks after finishing the instruction.  To see the instruction that was executing at the time of the event, use ```HIST``` command.

```BRKON```と```CBRKON```で指定できるイベントです。イベントが発生すると、CPUのインストラクションの実行後にブレークします。イベント発生時に実行していたインストラクションポインタを知るには```HIST```コマンドを使ってください。

```
ICW1
ICW4
```
Break on write to ICW1 or ICW4 of PIC.

PICのICW1またはICW4への書き込みでブレーク。

```
DMACREQ
```
Break on DMAC request.

DMACリクエストでブレーク。

```
FDCCMD
```
Break on Floppy-disk controller command.

フロッピーディスクコントローラのコマンドでブレーク。

```
CDCCMD
```
Break on internal CD-ROM controller command.

内蔵CD-ROMコントローラのコマンドでブレーク。


```
INT n
INT n AH=hh
INT n AX=hhhh
```
Break on interrupt.  n is a hexadecimal number.  Can specify specific value of AH or AX as a breaking condition.

割り込み発生でブレーク。nは16進数で指定。特定のAHまたはAXの値でブレークするような指定も可能。

```
RDCVRAM
```
```
WRCVRAM
```
```
RDFMRVRAM
```
```
WRFMRVRAM
```
```
IOR ioport
```
```
IOW ioport
```
```
VRAMR
```
```
VRAMW
```
```
VRAMRW
```
```
LBUTTONUP
```
Break on left-button up.

左ボタンリリースでブレーク。

```
RETKEY
RETURNKEY
```
Break on return-key press.

リターンキーでブレーク。

```
SCSICMD
```
Break on SCSI command.

SCSI commandでブレーク。

```
SCSIDMA
```
Break on SCSI DMA transfer.

SCSI DMA転送でブレーク。


```
MEMREAD physAddr
MEMR physAddr
```
Break on memory read.  The address is physicall address.  Use ```ADTR``` command to translate SEGMENT:ADDRESS to physical address.

メモリ読み込みでブレーク。アドレスは物理アドレスで指定。```ADTR```コマンドでセグメント:アドレスから物理アドレスに変換できます。

```
MEMWRITE physAddr
MEMW physAddr
```
Break on memory write.  The address is physicall address.  Use ```ADTR``` command to translate SEGMENT:ADDRESS to physical address.

メモリ書き込みでブレーク。アドレスは物理アドレスで指定。```ADTR```コマンドでセグメント:アドレスから物理アドレスに変換できます。
