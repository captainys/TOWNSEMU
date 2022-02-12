alltargets : exe\physdump.exe exe\dosntfy.exe exe\tgdrv.com exe\tgdrv.sys exe\biosver.exe



build\physdump.obj : physdump.asm print.asm rdhigh.asm wrhigh.asm gdt.asm
	masm physdump.asm build\physdump.obj build\physdump.lst NUL

exe\physdump.exe : build\physdump.obj
	link build\physdump.obj;
	copy physdump.exe exe\.
	del physdump.exe



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
