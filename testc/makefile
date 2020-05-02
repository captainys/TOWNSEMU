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

build\gentest.exp : build\gentest.OBJ build\f6f7.OBJ build\test_r_i.OBJ build\aadaam.OBJ build\btx.OBJ build\imul.OBJ build\bitshift.OBJ build\cbw_cwde.OBJ
	386link @cputest\gentest.LN

cputest\TESTCASE.H : build\gentest.exp
	run386 build\gentest.exp

build\runtest.OBJ : cputest\runtest.c cputest\TESTCASE.H io.h vmif.h
	run386 c:\hc386\bin\hcd386p cputest\runtest.c -obj build\runtest.OBJ

build\runtest.exp : build\io.obj build\vmif.obj build\runtest.OBJ build\f6f7.OBJ build\test_r_i.OBJ build\aadaam.OBJ build\btx.OBJ build\imul.OBJ build\bitshift.OBJ build\cbw_cwde.OBJ
	386link @cputest\runtest.LN



build\tfrio.obj : filetfr\TFRIO.ASM
	386asm filetfr\TFRIO.ASM -OBJECT build\tfrio.OBJ

build\ftclient.obj : filetfr\FTCLIENT.C
	hcd386 filetfr\FTCLIENT.C -obj build\ftclient.obj

build\ftclient.exp : build\io.obj build\vmif.obj build\tfrio.obj build\ftclient.obj
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
