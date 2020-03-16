FM TOWNS Emulator "Tsugaru"
FM TOWNS エミュレータ 「津軽」
by CaptainYS
http://www.ysflight.com
PEB01130@nifty.com



[Introduction]
It is an emulator of legendary Fujitsu FM TOWNS computer.  The goal is to emulate model II MX, which was the last computer I pledged allegiance.  Also I am trying to find and document undocumented features of FM TOWNS system as much as possible while writing the emulator.

Have been tested on Windows 10 and macOSX.


伝説の名機富士通FM TOWNSのエミュレータです。目標はモデルII MXを再現することです。FM TOWNS II MXは僕が最後に忠誠を誓った機種でした。また、開発と並行してFM TOWNSのシステムで未公開の機能を見つけて記録に残すことも目標にしています。

とりあえず、Windows 10とmacOSXで動きます。





[ROMS]
The ROM set is compatible with the best FM TOWNS emulator UNZ.

ROMイメージは現在最高のFM TOWNSエミュレータとされているUNZと互換性があります。





[LIMITATIONS]
At this point, all it can do is to start Towns MENU and run some programs.

80486 emulation is very slow.  At this time, for every byte of instruction, the CPU core is calcualting linear address, translating to physical address using page table.  Same for stack.  There is a large room for speed up.  Will be done down the road.

Not-all instructions of Intel 80486 processor have been implemented yet.  Towns OS didn't use task registers.  I have no plan on adding support for those registers.  Since the emulator works as a debugger, debug registers won't be supported, either.

Sprites are not supported yet.

FM/PCM Sound Generators are not supported yet.(YM2612 timer is supported because it is used for some other purposes.)

CDDA is not supported yet.

Right now only 1.23MB floppy disk image is supported.  Not writeable yet.



現時点では、Towns MENUを起動して、その下のプログラムをいくつか起動できるだけです。

80486エミュレーションが遅いです。今のバージョンではすべてのインストラクションのすべてのバイトについて、線形アドレスを計算して、ページテーブルを参照して物理アドレスに変換というプロセスを通ってます。スタックも同じです。だから、かなりのスピードアップの余地があります。そのうちやります。

80486のすべてのインストラクションをエミュレートできてません。なお、Towns OSはタスク機能は使ってなかったと思われるので多分サポートしません。また、エミュレータがデバッガとして機能するのでデバッグ機能もサポートしない予定です。

スプライトはまだ対応してません。

FM/PCM音源も対応してません。(YM2612のタイマーだけは他のいろんな機能で使っているので対応)

CDDAはまだ対応してません。

フロッピーディスクは1.23Mフロッピーディスクの読み込みしか対応してません。




[Source Code]
Open Source with 3-clause BSD License.




[Starting the Program]
It starts from terminal/command prompt.  The basic options are:

main_cui ROM_FILE_PATH -CD CD_IMAGE_FILE -FD0 FD0_IMAGE_FILE

CD-image can be .ISO or .CUE.  Floppy-disk image can be raw-binary or .D77 file.  You can look into scripts sub-directories for samples of other options.



コマンドから起動します。

main_cui ROM_FILE_PATH -CD CD_IMAGE_FILE -FD0 FD0_IMAGE_FILE

CD-imageは.ISOと.CUEに対応。ディスクイメージは.D77か生イメージファイルに対応。scriptsサブディレクトリ内にバッチ実行などの例題があるので、そっちも参照。ヘルプを書く時間が無い。




[Mouse Integration]
FM TOWNS uses a MSX mouse, which sends a displacement infornation in X- and Y-directions to the PC.  Therefore, mouse does not know where the cursor is.  To match the mouse cursor location in the host OS and guest VM, the emulator needs to know where the guest VM is thinking the mouse cursor is currently.

I did reverse engineering and found the mouse-cursor locations for TBIOSes that come with V1.1 L10/L20/L30, V2.1 L10B/L20/L31.  Will add other versions of TownsOS and VINGBIOS hopefully.  Most likely I will forget updating version numbers here, but probably I'll add comments in the commit log.


FM TOWNSはMSXマウスを使用します。MSXマウスはマウスの移動量のXYをPCに送信します。だから、マウスカーソルがPC上のどこにあるのか、マウスは知りません。ホストPCとゲストVMでマウス位置を一致させるためには、エミュレータが、VMが今マウスがどこにあると思っているか検出しなくてはなりません。

このエミュレータでは、Towns OS V1.1 L10/L20/L30, V2.1 L10B/L20/L31付属のTBIOSについて解析して、対応しました。他のバージョンやVINGBIOSにも順に対応しますが、多分ここのREADMEには書くのを忘れるでしょう。が、GITのコミットコメントには多分書くと思うのでそっちを見てください。





[Revisions]
2020/03/16
Make it public.

2020/03/04
Major progress!  My Towns can boot from ISO image of Towns OS V2.1 L20 and can move mouse cursor with keys.

2020/02/26
Major progress!  My Towns can read from ISO image!

2020/02/14
Succeeed in booting into Towns OS V2.1 L30 command mode, and then running a test program compiled by High-C!

2020/01/17
First line of code!

