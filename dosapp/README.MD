# TGDRV.COM: Host-VM File Sharing Driver for FM TOWNS Emulator "Tsugaru"
# TGDRV.COM: FM TOWNS エミュレータ 「津軽」用 ホスト-VM ファイル共有ドライバ

by CaptainYS

<http://www.ysflight.com>

PEB01130@nifty.com


## Introduction
This driver enables host-VM file sharing in FM TOWNS Emulator "Tsugaru."


## はじめに
このドライバは、FM TOWNSエミュレータ「津軽」でホスト-VM間のファイル共有を可能にします。


## Usage
If you run TGDRV.COM with no arguments, it will assign available drive letters to the shared directories.

If you want to specify drive letters, you can use /D option.  For example,

```
TGDRV.COM /D:P
```
will assign the first shared directory to VM P drive.  It will not assign the drive letter if the drive letter is already used.  You can specify multiple of this option to assign more drives.

If you use /F option, for example,
```
TGDRV.COM /F:P
```
it will re-assign a drive letter to the shared drive if the drive is already used.

/F option is useful in Towns OS V2.1 L31 and earlier.  Before Towns OS V2.1 L51, TMENU.EXP/TMENU.EXG 