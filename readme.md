# FM TOWNS Emulator "Tsugaru"
# FM TOWNS エミュレータ 「津軽」
by CaptainYS

<http://www.ysflight.com>

PEB01130@nifty.com

# Introduction
It is an emulator of legendary Fujitsu FM TOWNS computer.  The goal is to emulate model II MX, which was the last computer I pledged allegiance.  Also I am trying to find and document undocumented features of FM TOWNS system as much as possible while writing the emulator.

Have been tested on Windows 10 and macOS.  I'll try Linux when Windows and macOS versions get stable.


伝説の名機富士通FM TOWNSのエミュレータです。目標はモデルII MXを再現することです。FM TOWNS II MXは僕が最後に忠誠を誓った機種でした。また、開発と並行してFM TOWNSのシステムで未公開の機能を見つけて記録に残すことも目標にしています。

とりあえず、Windows 10とmacOSXで動きます。Linuxも対応するつもり。





# ROMS
The ROM set is compatible with the best FM TOWNS emulator UNZ.

ROMイメージは現在最高のFM TOWNSエミュレータとされているUNZと互換性があります。





# LIMITATIONS
At this point, all it can do is to start Towns MENU and run some programs.

The biggest set back right now is its speed.  How other DOS emulators are running a CPU that fast?  Also it is not in sync with real time.  I'm working on it.

80486 emulation is slow.  At this time, for every instruction, the CPU core is calcualting linear address, translating to physical address using page table.  Same for stack.  There is a large room for speed up.  Will be done down the road.

Not-all instructions of Intel 80486 processor have been implemented yet.  Towns OS didn't use task registers.  I have no plan on adding support for those registers.  Since the emulator works as a debugger, debug registers won't be supported, either.

RF5C68 PCM Sound Generator support is very primitive.

FM Sound Generators can play a tone, but since the VM is not in sync with the real time, it cannot play a music.  Also special mode for 3CH and 6CH is not supported, DETUNE, and other effects are not supported yet.(YM2612 timer is supported because it is used for some other purposes.)




現時点では、Towns MENUを起動して、その下のプログラムをいくつか起動できるだけです。

現状最大の問題は、CPUのエミュレーションのスピードです。他のDOSエミュレータはどうやってるんだろう？それから、実時間とシンクロしてないので、ゲームとかまともにプレイできないと思います。今、そのへん調整中です。

80486エミュレーションが遅いです。今のバージョンではすべてのインストラクションについて、線形アドレスを計算して、ページテーブルを参照して物理アドレスに変換というプロセスを通ってます。スタックも同じです。だから、かなりのスピードアップの余地があります。そのうちやります。

80486のすべてのインストラクションをエミュレートできてません。なお、Towns OSはタスク機能は使ってなかったと思われるので多分サポートしません。また、エミュレータがデバッガとして機能するのでデバッグ機能もサポートしない予定です。

RF5C68 PCM音源の音は出ますが、まだ非常に限定的なサポートです。

FM音源は、一応音が出ますが、VMが実時間とシンクロしてないので音楽演奏になりません。また、基本的な発声しか対応してません。3CH, 6CHの特殊モードとかDETUNEとか現時点では無視されます。
(YM2612のタイマーだけは他のいろんな機能で使っているので対応)




# Source Code
Open Source with 3-clause BSD License.




# Starting the Program
It starts from terminal/command prompt.  The basic options are:
```
main_cui ROM_FILE_PATH -CD CD_IMAGE_FILE -FD0 FD0_IMAGE_FILE
```
CD-image can be .ISO or .CUE.  Floppy-disk image can be raw-binary or .D77 file.  You can look into scripts sub-directories for samples of other options.

You can see the help by typing:
```
main_cui -HELP
```

I add some command samples below.  Please see also command.md



コマンドから起動します。
```
main_cui ROM_FILE_PATH -CD CD_IMAGE_FILE -FD0 FD0_IMAGE_FILE
```
CD-imageは.ISOと.CUEに対応。ディスクイメージは.D77か生イメージファイルに対応。scriptsサブディレクトリ内にバッチ実行などの例題があるので、そっちも参照。ヘルプを書く時間が無い。

また、次のようにタイプするとヘルプを表示します。英語ですが。
```
main_cui -HELP
```

以下に起動コマンドの例をいくつか挙げます。command.mdファイルも参照してください。




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




# Mouse Integration
FM TOWNS uses a MSX mouse, which sends a displacement infornation in X- and Y-directions to the PC.  Therefore, mouse does not know where the cursor is.  To match the mouse cursor location in the host OS and guest VM, the emulator needs to know where the guest VM is thinking the mouse cursor is currently.

I did reverse engineering and found the mouse-cursor locations for TBIOSes that come with V1.1 L10/L20/L30, V2.1 L10B/L20/L31.  Will add other versions of TownsOS and VINGBIOS hopefully.  Most likely I will forget updating version numbers here, but probably I'll add comments in the commit log.


FM TOWNSはMSXマウスを使用します。MSXマウスはマウスの移動量のXYをPCに送信します。だから、マウスカーソルがPC上のどこにあるのか、マウスは知りません。ホストPCとゲストVMでマウス位置を一致させるためには、エミュレータが、VMが今マウスがどこにあると思っているか検出しなくてはなりません。

このエミュレータでは、Towns OS V1.1 L10/L20/L30, V2.1 L10B/L20/L31付属のTBIOSについて解析して、対応しました。他のバージョンやVINGBIOSにも順に対応しますが、多分ここのREADMEには書くのを忘れるでしょう。が、GITのコミットコメントには多分書くと思うのでそっちを見てください。




# Unit Tests
You need bootable floppy disk images/CD-ROM images to run unit tests.  My guess is it is ok to redistribute Towns OS bootable floppy disk images, but to be safe I am keeping them to myself.

If you have an ISO image (track 0 image) of Towns OS V1.1 L10 or Towns OS V2.1 L31, you can run two tests by:

1. Configure the top-level CMakeLists.txt so that DISC_IMAGE_DIR points to the directory where you store your ISO images.
2. CMake and build everything.
3. Run either one of the following comman in the build directory.
```
    ctest -R MX_V21L31_GUI
    ctest -R MX_V11L10_GUI
```
The tests are timing-sensitive, or the CPU needs to be reasonably fast.  If not, mouse-click may take place before wigets are ready, in which case, the test may not run all the way.

ユニットテストを実行するには、起動可能なフロッピーディスクイメージまたはCD-ROMイメージが必要です。Towns OSでフォーマットした起動可能ディスクイメージは多分再配布しても良いと思いますが(多分富士通としては小さなソフトハウスなどがフロッピー版のソフトを販売するために使うことを想定していたのではないかと思うので)、一応、ディスクイメージは外に出してません。

が、Towns OS V1.1 L10またはV2.1 L31のISOイメージ(トラック0のイメージ)があれば、GUIのテストだけ実行できます。

1. 最上位のCMakeLists.txtを修正して、DISC_IMAGE_DIRがISOイメージの場所を指すようにする。
2. CMakeを再度実行してからBuild。
3. 次のコマンドを実行。
```
    ctest -R MX_V21L31_GUI
    ctest -R MX_V11L10_GUI
```

なお、タイミングに影響されるので遅いCPUだと多分途中から先に進まなくなって、テスト途中で止まる鴨しれません。




# Revisions
### 2020/06/15
- Added separate timer for audio.  Super DAISENRYAKU BGM plays better.  Still need more work in YM2612 emulation.
- Changed the internal clock frequency of YM2612 from 600KHz to 690KHz.  FM Towns Technical Databook [2] tells it is 600KHz, but somehow 690KHz is giving me more accurate timing.
- Fixed CDC.  Was reporting number of tracks as binary.  It should be in BCD.  TOWNS ILLUSION runs again.  This time it needs to start from BIN/CUE.  It stopps in the middle if you start from ISO.
- Speed up in CPU emulation.

### 2020/06/12
- Better CDDA emulation, preliminary support for electric volume registers, fixed BT MEM,R instruction.  Confirmed RAYXANBER can start game play!

### 2020/06/07
- Improvement in YM2612 emulation.  Still different, but getting similar.  (Struggling to make sense of envelope calculation.)
- Fixed hard-disk read/write.  Now can install TownsOS in a hard-disk image, and boot from it.

### 2020/05/26
- Mouse Integration for Wing Commander.  Need -APP WINGCOMMANDER1 option.
- Fixed CDC.  Was pushing status code 06 00 00 00 twice after MODE1 Read.
- Added disk-access indicators.  Changed FDC timing.

### 2020/05/23
- Tested TownsOS boot and mouse integration with Freeware Collection 1-3, Free Software Collection 4-11.  Made corrections for Free Software Collection 4 Disc B and Free Software Collection 6.
- Free Software Collection 4 Disc B uses Towns OS V2.1L10 (L10 with no mark).  DOS Extender that comes with Towns OS V2.1L10 blocks unless the CPU type identified by I/O port 0030H is 80386DX or 80386SX.  For this problem, I have added -PRETEND386DX option.

### 2020/05/19
- Sprite emulation should be pretty good.
- Game pad can be emulated by keyboard (-GAMEPORT0 option.  Use arrow keys and Z,X,A,S)
- Afterburner II (SEGA, ported by CRI) is playable!  Sample boot script is Afterburner2.py.

### 2020/05/06
- Preliminary support for SCSI Hard disks.  (Create a big binary filled with zeros and use -HD0 option.)

### 2020/05/03
- CPU core speed up.  Still not good enough for 486DX 66MHz.
- Support 3-mode floppy disk read/write.
- Very preliminary YM2612 support.
- Added initial CMOS.

### 2020/04/11
- Started CPU instruction tests.  Already captured and fixed numerous bugs in CPU cores.
- Fixed sprite.
- Towns ILLUSION works all the way without corrupted image!!!!

### 2020/04/10
- At least the program doesn't crash or freeze when sprite is used.
- Towns ILLUSION (Legendary DEMO that comes with Towns OS V1.1 L10) runs all the way, although the images are still corrupted.

### 2020/04/09
- Primitive support for RF5C68 PCM Sound Generator Playback.
- Roughly 25% speed up.

### Somewhere in between
- Supported CDDA from BIN/CUE
- Added a unit test for Towns OS GUI V1.1 L30

### 2020/03/22
- Added unit tests for Towns OS GUI V1.1 L10 and V2.1 L31.

### 2020/03/16
- Make it public.

### 2020/03/04
- Major progress!  My Towns can boot from ISO image of Towns OS V2.1 L20 and can move mouse cursor with keys.

### 2020/02/26
- Major progress!  My Towns can read from ISO image!

### 2020/02/14
- Succeeed in booting into Towns OS V2.1 L30 command mode, and then running a test program compiled by High-C!

### 2020/01/17
- First line of code!



# Mystery
- Interpretation of I/O 480H

[2] pp. 91 tells that:
```
I/O 0480H
Bit 1: Select F8000H to FFFFFH mapping RAM or System-ROM (0:SysROM  1:RAM)
Bit 0: RAM or CMOS (0:RAM  1:CMOS)
```
It doesn't tell where in the memory space Bit 0 is controlling.  From the memory map, it looks to be D0000 to DFFFF.  However, the boot ROM does not clear Bit 0 before memory test, which causes CMOS destruction upon restarting by ```REIPL.COM```.

Only interpretation I can think of is:
```
Bit 1   Bit 0    F0000-FFFFF   D0000-DFFFF
  0       0       SysROM        RAM
  0       1       SysROM        CMOS
  1       0       RAM           RAM
  1       1       RAM           RAM
```
Eventually I'm going to write a test program and see the behavior on my actual FM Towns.


# References
[1] Intel i486TM Microprocessor Programmer's Reference Manual, Intel, 1990

[2] Noriaki Chiba, FM TOWNS Technical Databook, 3rd Edition, ASCII, 1994

[3] towns_cd.h, Linux for FM TOWNS source code.

[4] X86 Opcode and Instruction Reference Home, http://ref.x86asm.net/coder32.html (As of February 9 2020)

[5] Intel 80386 Programmre's Reference Manual, Intel, 1986

[6] https://github.com/nabe-abk/free386/blob/master/doc-ja/dosext/coco_nsd.txt

[7] http://www.ctyme.com/rbrown.htm

[8] SEGA Genesis Software Manual

[9] http://www.mit.edu/afs/sipb.mit.edu/contrib/doc/specs/protocol/scsi-2/s2-r10l.txt (As of May 3 2020)

[10] List of Floppy Disk Formats, https://en.wikipedia.org/wiki/List_of_floppy_disk_formats, (As of May 23 2020)

[11] YM2608 OPNA Application Manual

[12] Source code of Artane's FM Towns emulator project https://github.com/Artanejp
