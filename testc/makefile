build\io.obj : io.asm
	386asm io.asm -OBJECT build\io.obj

build\vmif.obj : vmif.c
	hcd386 vmif.c -obj build\vmif.obj



build\gentest.OBJ : cputest\gentest.c
	hcd386 cputest\gentest.c -obj build\gentest.OBJ

build\imul.OBJ : cputest\imul.asm
	386asm cputest\imul.asm -OBJECT build\imul.OBJ

build\f6f7.OBJ : cputest\f6f7.asm
	386asm cputest\f6f7.asm -OBJECT build\f6f7.OBJ

build\test_r_i.OBJ : cputest\test_r_i.asm
	386asm cputest\test_r_i.asm -OBJECT build\test_r_i.OBJ

build\aadaam.OBJ : cputest\aadaam.asm
	386asm cputest\aadaam.asm -OBJECT build\aadaam.OBJ

build\btx.OBJ : cputest\btx.asm
	386asm cputest\btx.asm -OBJECT build\btx.OBJ

build\bitshift.OBJ : cputest\bitshift.asm
	386asm cputest\bitshift.asm -OBJECT build\bitshift.OBJ

build\cbw_cwde.OBJ : cputest\cbw_cwde.asm
	386asm cputest\cbw_cwde.asm -OBJECT build\cbw_cwde.OBJ

build\adcsbbsp.OBJ : cputest\adcsbbsp.asm
	386asm cputest\adcsbbsp.asm -OBJECT build\adcsbbsp.OBJ

build\adcsbb.OBJ : cputest\adcsbb.asm
	386asm cputest\adcsbb.asm -OBJECT build\adcsbb.OBJ

build\bt_mem_r.OBJ : cputest\bt_mem_r.asm
	386asm cputest\bt_mem_r.asm -OBJECT build\bt_mem_r.OBJ

build\daadas.OBJ : cputest\daadas.asm
	386asm cputest\daadas.asm -OBJECT build\daadas.OBJ

build\mov.OBJ : cputest\mov.asm
	386asm cputest\mov.asm -OBJECT build\mov.OBJ

build\shldshrd.OBJ : cputest\shldshrd.asm
	386asm cputest\shldshrd.asm -OBJECT build\shldshrd.OBJ

build\binaryop.OBJ : cputest\binaryop.asm
	386asm cputest\binaryop.asm -OBJECT build\binaryop.OBJ

build\gentest.exp : build\gentest.OBJ build\f6f7.OBJ build\test_r_i.OBJ build\aadaam.OBJ build\btx.OBJ build\imul.OBJ build\bitshift.OBJ build\cbw_cwde.OBJ build\adcsbbsp.OBJ build\bt_mem_r.OBJ build\daadas.OBJ build\mov.OBJ build\shldshrd.OBJ build\binaryop.OBJ
	386link @cputest\gentest.LN

cputest\TESTCASE.H : build\gentest.exp
	run386 build\gentest.exp

build\runtest.OBJ : cputest\runtest.c cputest\TESTCASE.H io.h vmif.h
	run386 c:\hc386\bin\hcd386p cputest\runtest.c -obj build\runtest.OBJ

build\runtest.exp : build\io.obj build\vmif.obj build\runtest.OBJ build\f6f7.OBJ build\test_r_i.OBJ build\aadaam.OBJ build\btx.OBJ build\imul.OBJ build\bitshift.OBJ build\cbw_cwde.OBJ build\adcsbbsp.OBJ build\bt_mem_r.OBJ build\daadas.OBJ build\mov.OBJ build\shldshrd.OBJ build\binaryop.OBJ
	386link @cputest\runtest.LN



build\tfrio.obj : filetfr\TFRIO.ASM
	386asm filetfr\TFRIO.ASM -OBJECT build\tfrio.OBJ

build\ftclient.obj : filetfr\FTCLIENT.C
	hcd386 filetfr\FTCLIENT.C -obj build\ftclient.obj

build\ftclient.exp : build\io.obj build\vmif.obj build\tfrio.obj build\ftclient.obj filetfr\FTCLIENT.LN
	386link @filetfr\FTCLIENT.LN




build\FAIL.EXE : FAIL.ASM
	MASM FAIL.ASM build\FAIL.OBJ build\FAIL.LST NUL
	LINK build\FAIL.OBJ;
	COPY FAIL.EXE build\.
	DEL FAIL.EXE



build\SUCCESS.EXE : SUCCESS.ASM
	MASM SUCCESS.ASM build\SUCCESS.OBJ build\SUCCESS.LST NUL
	LINK build\SUCCESS.OBJ;
	COPY SUCCESS.EXE build\.
	DEL SUCCESS.EXE



build\captcrtc.obj : captcrtc.c io.h vmif.h
	hcd386 captcrtc.c -obj build\captcrtc.obj

build\captcrtc.exp : build\captcrtc.obj build\io.obj build\vmif.obj
	386link build\captcrtc.obj build\io.obj -lib C:\hc386\small\hce.lib -lib C:\hc386\townslib\lib\tbios.lib -exe build\captcrtc.exp



build\test01.obj : test01.c io.h vmif.h
	hcd386 test01.c -obj build\test01.obj

build\test01.exp : build\test01.obj build\vmif.obj
	386link build\test01.obj -lib C:\hc386\small\hce.lib -exe build\test01.exp



build\test02.obj : test02.c io.h vmif.h
	hcd386 test02.c -obj build\test02.obj

build\test02.exp : build\test02.obj build\vmif.obj
	386link build\test02.obj -lib C:\hc386\small\hce.lib -exe build\test02.exp



build\test03.obj : test03.c io.h vmif.h
	hcd386 test03.c -obj build\test03.obj

build\test03.exp : build\test03.obj build\vmif.obj
	386link build\test03.obj -lib C:\hc386\small\hce.lib -exe build\test03.exp



build\test04.obj : test04.c io.h vmif.h
	hcd386 test04.c -obj build\test04.obj

build\test04.exp : build\test04.obj build\vmif.obj
	386link build\test04.obj -lib C:\hc386\small\hce.lib -exe build\test04.exp



build\test05.obj : test05.c io.h vmif.h
	hcd386 test05.c -obj build\test05.obj

build\test05.exp : build\test05.obj build\vmif.obj
	386link build\test05.obj -lib C:\hc386\small\hce.lib -exe build\test05.exp



build\test06.obj : test06.c io.h vmif.h
	hcd386 test06.c -obj build\test06.obj

build\test06.exp : build\test06.obj build\vmif.obj
	386link build\test06.obj -lib C:\hc386\small\hce.lib -exe build\test06.exp



build\test07.obj : test07.c io.h vmif.h
	hcd386 test07.c -obj build\test07.obj

build\test07.exp : build\test07.obj build\vmif.obj
	386link build\test07.obj -lib C:\hc386\small\hce.lib -exe build\test07.exp



build\test08.obj : test08.c io.h vmif.h
	hcd386 test08.c -obj build\test08.obj

build\test08.exp : build\test08.obj build\io.obj build\vmif.obj
	386link build\test08.obj -exe build\test08.exp @LIBS.LN



build\testcd.obj : testcd.c io.h vmif.h
	hcd386 testcd.c -obj build\testcd.obj

build\testcd.exp : build\testcd.obj build\io.obj build\vmif.obj
	386link build\testcd.obj -exe build\testcd.exp @LIBS.LN



build\egb01.obj : egb01.c io.h vmif.h
	hcd386 egb01.c -obj build\egb01.obj

build\egb01.exp : build\egb01.obj build\io.obj build\vmif.obj
	386link build\egb01.obj -exe build\egb01.exp @LIBS.LN



build\egb02.obj : egb02.c io.h vmif.h
	hcd386 egb02.c -obj build\egb02.obj

build\egb02.exp : build\egb02.obj build\io.obj build\vmif.obj
	386link build\egb02.obj -exe build\egb02.exp @LIBS.LN



build\mos01.obj : mos01.c io.h vmif.h
	hcd386 mos01.c -obj build\mos01.obj

build\mos01.exp : build\mos01.obj build\io.obj build\vmif.obj
	386link build\mos01.obj -exe build\mos01.exp @LIBS.LN



build\sprite01.obj : sprite01.c io.h vmif.h
	hcd386 sprite01.c -obj build\sprite01.obj

build\sprite01.exp : build\sprite01.obj build\io.obj build\vmif.obj
	386link build\sprite01.obj -exe build\sprite01.exp @LIBS.LN



build\timer.obj : timer.c io.h vmif.h
	hcd386 timer.c -obj build\timer.obj

build\timer.exp : build\timer.obj build\io.obj build\vmif.obj
	386link build\timer.obj -exe build\timer.exp @LIBS.LN



build\vramtest.obj : vram\vramtest.asm
	386asm vram\vramtest.asm -OBJECT build\vramtest.obj

build\vram.OBJ : vram\vram.c
	hcd386 vram\vram.c -obj build\vram.OBJ

build\vramtest.exp : build\vram.OBJ build\vramtest.OBJ vram\vramtest.ln
	386link @vram\vramtest.LN



build\readtoc.obj : cdrom\readtoc.c
	hcd386 cdrom\readtoc.c -obj build\readtoc.OBJ

build\readtoc.exp : build\readtoc.OBJ build\io.OBJ
	386link @cdrom\readtoc.LN


build\ym2612FB.obj : ym2612FB.c
	hcd386 ym2612FB.c -obj build\ym2612FB.OBJ

build\ym2612FB.exp : build\ym2612FB.OBJ build\io.OBJ
	386link build\ym2612FB.obj -exe build\ym2612FB.exp @LIBS.LN



build\repeat.obj : cdrom\repeat.asm
	MASM cdrom\repeat.ASM build\repeat.OBJ build\repeat.LST NUL

build\repeat.exe : build\repeat.OBJ
	LINK build\repeat.OBJ;
	COPY repeat.EXE build\.
	DEL repeat.EXE

build\cdda.obj : cdrom\cdda.asm
	MASM cdrom\cdda.ASM build\cdda.OBJ build\cdda.LST NUL

build\cdda.exe : build\cdda.OBJ
	LINK build\cdda.OBJ;
	COPY cdda.EXE build\.
	DEL cdda.EXE



build\vndrvtst.obj : vndrvtst.asm
	386asm vndrvtst.asm -OBJECT build\vndrvtst.obj

build\vndrvbrk.obj : vndrvbrk.c
	hcd386 vndrvbrk.c -obj build\vndrvbrk.obj

build\vndrvena.obj : vndrvena.c
	hcd386 vndrvena.c -obj build\vndrvena.obj

build\vndrvpri.obj : vndrvpri.c
	hcd386 vndrvpri.c -obj build\vndrvpri.obj

build\vndrvdir.obj : vndrvdir.c
	hcd386 vndrvdir.c -obj build\vndrvdir.obj

build\vndrvbrk.exp : build\vndrvbrk.obj  build\vndrvtst.obj
	386link build\vndrvbrk.obj build\vndrvtst.obj @LIBS.LN

build\vndrvena.exp : build\vndrvena.obj  build\vndrvtst.obj
	386link build\vndrvena.obj build\vndrvtst.obj @LIBS.LN

build\vndrvpri.exp : build\vndrvpri.obj  build\vndrvtst.obj
	386link build\vndrvpri.obj build\vndrvtst.obj @LIBS.LN

build\vndrvdir.exp : build\vndrvdir.obj  build\vndrvtst.obj
	386link build\vndrvdir.obj build\vndrvtst.obj @LIBS.LN
