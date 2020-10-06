# FM TOWNS Hard Disk Imaging
# FM TOWNS ハードディスクのイメージ化


## [はじめに]
FM TOWNSのエミュレータは既に非常に高い再現性を持つUNZがあり、僕が津軽を作ってて、Common Source Projectベースのエミュレータも開発されつつあります。昔使ったソフトも保存してあれば使えるし、ある程度ヤフオクで落とせたり、レトロコンピュータショップから買ってくることもできます。しかし、当時使っていたハードディスクとなると、エミュレータ上に持ってくる手段がありませんでした。というか、やってた人はいるかもしれないけどあまり一般的ではありませんでした。しかも、最近ではSCSIハードディスクをPCに接続すること自体が難しくなりつつあります。ハードディスクイメージ自体はTOWNSのBIOS INT 93Hを使えば簡単に読み出せるのですが、それをTOWNSの外に持ってくる手段が問題でした。

この問題を仮想SCSIデバイスを使って解決する方法を開発しました。使用可能なFM TOWNS本体、イメージ化するハードディスク、それと仮想SCSIハードディスクを用意すれば、あとはソフトウェアでなんとかなります。発送としては、まずハードディスクイメージを仮想SCSIディスク上のファイルにセーブして、仮想SCSIディスクの保存メディアのイメージを作成、作成したイメージを津軽にマウント、津軽からWindowsなりLinuxなりMacなりにファイルを転送、最後につなぎ合わせて完成となります。

なお、直接仮想SCSIディスクにディスクのコピーを作るという方法も考えました。そして、これは実現可能です。しかし、ハードディスクから仮想SCSIディスクにコピーしようとしてうっかり仮想SCSIディスクの内容で肝心のディスクドライブを上書きしちゃったという事故が容易に想像できるので、まわりくどいですが、いったんファイルに保存する方法にしました。


## [必要なもの]
1. コンソールモードに入って操作可能なFM TOWNS本体
2. TOWNSシステムソフトウェア
3. イメージ化するハードディスク
4. 「津軽」実行環境
5. 仮想SCSIハードドライブ（SCSI2SDで確認済み)
6. 3モードフロッピーディスクドライブなど、TOWNS本体にイメージ化プログラムを転送する手段



## [準備]
### 仮想SCSIデバイス
まず、SCSI2SDなど仮想SCSIデバイスを使えるようにする必要があります。これは、各デバイスごとの説明を参照してください。なお、仮想ディスクドライブのサイズですが、当時のTOWNSで使っていたディスクドライブのセクタをすべてファイルとして保存できるだけのサイズを設定してください。多分当時のハードディスクのサイズだったら2GBもあれば絶対大丈夫だと思います。

また、津軽を使ってハードディスクイメージを取り出すために、FTCLIENT.EXPを使います。townsapp/exp/FTCLIENT.EXPにあるので、これをフロッピーディスクイメージなどにセーブしておいてください。

### TOWNS実機側の設定
イメージを取るTOWNS実機では、SCSI2SDのディスクドライブを認識できることを確認してください。そして、TOWNSのファイルシステムでは最大32MBのファイルしか作ることができないので、イメージを分割してセーブする必要があります。しかも、DOS6環境が無ければひとつのドライブは127MB制限がかかっているので、複数ドライブを設定する必要があります。

以下の説明では、イメージを取るディスクドライブのSCSI IDが0で、D,E,F,Gドライブが割り当てられていて、仮想SCSIディスクはSCSI IDが1、パーティションが6分割で、すべてのパーティションが127MBで、H,I,J,K,L,Mドライブに割り当てられているという前提で書きます。バッチファイルなどをそのまま使うためには、Townsシステムソフトウェアを使って同じようにディスクドライブを割り当ててください。

なお、当然仮想SCSIディスクドライブを初期化する必要がありますが、このとき絶対に間違ってイメージを取る方のドライブを初期化しないように気を付けてください。物理的につながってなければ間違って初期化することもないので、仮想SCSIディスクドライブを初期化するときは、できればイメージを取る元のドライブをはずしておくことをおすすめします。

書いてて思ったんだけど、「仮想」じゃないのか。なんて言うのが適切なのかな。SCSIエミュレータ？ ハードディスクをSDカードを使って仮想化しているとは言えるけど、これってただのSSDな気がしてきた。



## [HDREAD.EXPとバッチファイルをTOWNS実機に持っていく]
TOWNS IIであれば、フロッピーディスクを一枚1.44MBにフォーマットして、townsapp/exp/HDREAD.EXP、それとイメージ取り用バッチファイルをコピーすれば済みます。また、バッチファイルをそのまま使うには、RUN386.EXEも同じフロッピーディスクにコピーしておくと良いです。

フロッピーディスクが使えない場合、なんとしてもTOWNS本体に、HDREAD.EXPを持っていかなくてはなりません。RS232CのクロスケーブルでつないでWINK2等のターミナルモードでXMODEMを使うなど、工夫してください。多分USB接続の3モードフロッピードライブとフロッピーディスクを1枚入手する方が速いと思います。なお、2020/09/11現在では、3モードドライブさえあれば、Windows10で1.23MBフォーマット、1.44MBフォーマットのフロッピーディスクの読み書きができました。ただ、1.23MBフロッピーディスクのフォーマットだけできません。

以下に、イメージ作成バッチファイルの例を書きます。

```
REM 30MB at a time
\run386 -nocrt hdread h:\img0000.bin 0   0           60000
\run386 -nocrt hdread h:\img0001.bin 0   60000       60000
\run386 -nocrt hdread h:\img0002.bin 0   120000      60000
\run386 -nocrt hdread h:\img0003.bin 0   180000      60000
                                         
\run386 -nocrt hdread i:\img0004.bin 0   240000      60000
\run386 -nocrt hdread i:\img0005.bin 0   300000      60000
\run386 -nocrt hdread i:\img0006.bin 0   360000      60000
\run386 -nocrt hdread i:\img0007.bin 0   420000      60000
                                         
\run386 -nocrt hdread j:\img0008.bin 0   480000      60000
\run386 -nocrt hdread j:\img0009.bin 0   540000      60000
\run386 -nocrt hdread j:\img0010.bin 0   600000      60000
\run386 -nocrt hdread j:\img0011.bin 0   660000      60000
                                         
\run386 -nocrt hdread k:\img0012.bin 0   720000      60000
\run386 -nocrt hdread k:\img0013.bin 0   780000      60000
\run386 -nocrt hdread k:\img0014.bin 0   840000      60000
\run386 -nocrt hdread k:\img0015.bin 0   900000      60000
                                         
\run386 -nocrt hdread l:\img0016.bin 0   960000      60000
\run386 -nocrt hdread l:\img0017.bin 0   1020000     60000
\run386 -nocrt hdread l:\img0018.bin 0   1080000     60000
\run386 -nocrt hdread l:\img0019.bin 0   1140000     60000
                                         
\run386 -nocrt hdread m:\img0020.bin 0   1200000     60000
\run386 -nocrt hdread m:\img0021.bin 0   1260000     60000
\run386 -nocrt hdread m:\img0022.bin 0   1320000     60000
\run386 -nocrt hdread m:\img0023.bin 0   1380000     60000
```

この例では約720MBのディスクまでイメージが取れます。HDREAD.EXPのパラメータはイメージファイル名、読み込む元のSCSI ID、開始セクタ、セクタ数です。読み込み元がSCSI ID ０以外の場合はそれに合わせて変更してください。なお、この例では各ドライブに120MBずつイメージファイルを作ります。読み込みセクタが元のハードディスクの領域を超えたら以降は何もしないので、同じバッチスクリプトで小さなハードディスクも対応できます。また、別のドライブのRUN386.EXEを使う場合は、```\run386```を、適当な場所に変更してください。


## [実機でイメージ作成]
TOWNS本体で、コマンドモードに入ってください。そして、上の手順で作成したフロッピーディスクをAドライブ(あるいはどっちか生きてる方)に入れて次のコマンドをタイプします。(CDからコマンドモードに入ったとして)

```
Q> A:
A> HDREAD.BAT
```

HDREAD.BATは上の手順で作成したバッチファイルです。あとは、イメージ作成が終わるまでひたすら待ちます。


## [PCにイメージ転送]
イメージ作成が終わったら今度はイメージをPCに持ってきます。まずは、仮想SCSIドライブのメディアのイメージを取ります。僕はWin32DiskImagerを使ってますが、適当なイメージ化ソフトを使ってください。仮想SCSIドライブによっては、既にSDカード上のファイルになってるものがあるかもしれません。その場合は、ファイルを安全な場所にコピーするだけでOKです。



## [津軽を使ってファイル抜き出し]
PCに転送したイメージは、そのまま津軽でマウントできます。実機と同じようにSCSI ID 1番にマウントしたと仮定します。ファイルの転送にはCUIモードを使う必要があるので(GUIにも出せばいいんだけどまだやってない)、起動時のパラメータに、

```
-HD1 sdcardimage.bin
```

のような感じでマウントしてください。

津軽上でもTOWNSシステムソフトウェアを使って、SCSI ID 1に実機と同じドライブを割り当てます。実機と同じドライブを割り当てたら津軽CUI上で、以下のコマンドをタイプします。が、タイプは面倒なのでターミナルにコピーペーストするのがいいと思いますが。

```
VM2HOST h:\img0000.bin
VM2HOST h:\img0001.bin
VM2HOST h:\img0002.bin
VM2HOST h:\img0003.bin
                          
VM2HOST i:\img0004.bin
VM2HOST i:\img0005.bin
VM2HOST i:\img0006.bin
VM2HOST i:\img0007.bin
                          
VM2HOST j:\img0008.bin
VM2HOST j:\img0009.bin
VM2HOST j:\img0010.bin
VM2HOST j:\img0011.bin
                          
VM2HOST k:\img0012.bin
VM2HOST k:\img0013.bin
VM2HOST k:\img0014.bin
VM2HOST k:\img0015.bin
                          
VM2HOST l:\img0016.bin
VM2HOST l:\img0017.bin
VM2HOST l:\img0018.bin
VM2HOST l:\img0019.bin
                          
VM2HOST m:\img0020.bin
VM2HOST m:\img0021.bin
VM2HOST m:\img0022.bin
VM2HOST m:\img0023.bin
```

タイプしたら、今度は津軽のTOWNS上で、最初の方で作っておいたFTCLIENT.EXPの入ったディスクイメージをマウントして、

```
run386 -nocrt FTCLIENT.EXP
```

とタイプして、しばらく待つと、img????.binがPC側のカレントディレクトリにセーブされます。


## [イメージのつなぎ合わせ]
最後に抜き出したイメージをつなげて出来上がりです。簡単なのは、Windowsのコマンドプロンプトを使う方法ですね。(PowerShellだと使えない)例えば、img0000.bin～img0005.binまでにすべてが収まっていたとすると、

```
copy /B img0000.bin + img0001.bin + img0002.bin + img0003.bin + img0005.bin salvage.h0
```

これで完成。salvage.h0を津軽にマウントすると、実機ハードディスクと同じ中身が見えるはずです。一応、ディスクドライブふたつで成功しました。

なお、まだ津軽はOASYSの起動はできないようなのですが、現在鋭意対応を目指して調査中です。

