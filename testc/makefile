build\io.obj : io.asm
	386asm io.asm -OBJECT build\io.obj

build\FAIL.obj : FAIL.asm
	386asm FAIL.asm -OBJECT build\FAIL.obj



build\captcrtc.obj : captcrtc.c
	hcd386 captcrtc.c -obj build\captcrtc.obj

build\captcrtc.exp : build\captcrtc.obj build\io.obj
	386link build\captcrtc.obj build\io.obj -lib C:\hc386\small\hce.lib -lib C:\hc386\townslib\lib\tbios.lib -exe build\captcrtc.exp



build\test01.obj : test01.c
	hcd386 test01.c -obj build\test01.obj

build\test01.exp : build\test01.obj
	386link build\test01.obj -lib C:\hc386\small\hce.lib -exe build\test01.exp



build\test02.obj : test02.c
	hcd386 test02.c -obj build\test02.obj

build\test02.exp : build\test02.obj
	386link build\test02.obj -lib C:\hc386\small\hce.lib -exe build\test02.exp



build\test03.obj : test03.c
	hcd386 test03.c -obj build\test03.obj

build\test03.exp : build\test03.obj
	386link build\test03.obj -lib C:\hc386\small\hce.lib -exe build\test03.exp



build\test04.obj : test04.c
	hcd386 test04.c -obj build\test04.obj

build\test04.exp : build\test04.obj
	386link build\test04.obj -lib C:\hc386\small\hce.lib -exe build\test04.exp



build\test05.obj : test05.c
	hcd386 test05.c -obj build\test05.obj

build\test05.exp : build\test05.obj
	386link build\test05.obj -lib C:\hc386\small\hce.lib -exe build\test05.exp



build\test06.obj : test06.c
	hcd386 test06.c -obj build\test06.obj

build\test06.exp : build\test06.obj
	386link build\test06.obj -lib C:\hc386\small\hce.lib -exe build\test06.exp



build\test07.obj : test07.c
	hcd386 test07.c -obj build\test07.obj

build\test07.exp : build\test07.obj
	386link build\test07.obj -lib C:\hc386\small\hce.lib -exe build\test07.exp



build\test08.obj : test08.c
	hcd386 test08.c -obj build\test08.obj

build\test08.exp : build\test08.obj build\io.obj build\FAIL.obj
	386link build\test08.obj -exe build\test08.exp @LIBS.LN



build\egb01.obj : egb01.c
	hcd386 egb01.c -obj build\egb01.obj

build\egb01.exp : build\egb01.obj build\io.obj
	386link build\egb01.obj -exe build\egb01.exp @LIBS.LN
