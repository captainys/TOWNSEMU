alltargets : exe\physdump.exe exe\physdmpw.exe exe\physwrit.exe exe\physwriw.exe exe\physveri.exe exe\dosntfy.exe exe\tgdrv.com exe\tgdrv.sys exe\biosver.exe



build\physdump.obj : physdump.asm print.asm rdhigh.asm wrhigh.asm gdt.asm
	masm physdump.asm build\physdump.obj build\physdump.lst NUL

exe\physdump.exe : build\physdump.obj
	link build\physdump.obj;
	copy physdump.exe exe\.
	del physdump.exe



build\physdmpw.obj : physdmpw.asm print.asm rdhigh.asm wrhigh.asm gdt.asm
	masm physdmpw.asm build\physdmpw.obj build\physdmpw.lst NUL

exe\physdmpw.exe : build\physdmpw.obj
	link build\physdmpw.obj;
	copy physdmpw.exe exe\.
	del physdmpw.exe



build\physwrit.obj : physwrit.asm print.asm rdhigh.asm wrhigh.asm gdt.asm
	masm physwrit.asm build\physwrit.obj build\physwrit.lst NUL

exe\physwrit.exe : build\physwrit.obj
	link build\physwrit.obj;
	copy physwrit.exe exe\.
	del physwrit.exe



build\physwriw.obj : physwriw.asm print.asm rdhigh.asm wrhigh.asm gdt.asm
	masm physwriw.asm build\physwriw.obj build\physwriw.lst NUL

exe\physwriw.exe : build\physwriw.obj
	link build\physwriw.obj;
	copy physwriw.exe exe\.
	del physwriw.exe



build\physveri.obj : physveri.asm print.asm rdhigh.asm wrhigh.asm gdt.asm
	masm physveri.asm build\physveri.obj build\physveri.lst NUL

exe\physveri.exe : build\physveri.obj
	link build\physveri.obj;
	copy physveri.exe exe\.
	del physveri.exe



build\biosver.obj : biosver.asm print.asm
	masm biosver.asm build\biosver.obj build\biosver.lst NUL

exe\biosver.exe : build\biosver.obj
	link build\biosver.obj;
	copy biosver.exe exe\.
	del biosver.exe



build\dosntfy.obj : dosntfy.asm print.asm rdhigh.asm wrhigh.asm gdt.asm
	masm dosntfy.asm build\dosntfy.obj build\dosntfy.lst NUL

exe\dosntfy.exe : build\dosntfy.obj
	link build\dosntfy.obj;
	copy dosntfy.exe exe\.
	del dosntfy.exe



build\tgdrvcom.obj : tgdrvcom.asm tgdrvmain.asm tgdrvdef.asm tgdrvbuf.asm
	masm tgdrvcom.asm build\tgdrvcom.obj build\tgdrv.lst NUL

exe\tgdrv.com : build\tgdrvcom.obj
	link build\tgdrvcom.obj;
	exe2bin tgdrvcom.exe tgdrv.com
	copy tgdrv.com exe\.
	del tgdrvcom.exe
	del tgdrv.com



build\tgdrvsys.obj : tgdrvsys.asm tgdrvmain.asm tgdrvdef.asm tgdrvbuf.asm
	masm tgdrvsys.asm build\tgdrvsys.obj build\tgdrvsys.lst NUL

exe\tgdrv.sys : build\tgdrvsys.obj
	link build\tgdrvsys.obj;
	exe2bin tgdrvsys.exe tgdrv.sys
	copy tgdrv.sys exe\.
	del tgdrvsys.exe
	del tgdrv.sys
