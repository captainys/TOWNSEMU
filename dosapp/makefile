alltargets : exe\physdump.exe exe\dosntfy.exe exe\tgdrv.com exe\tgdrv.sys



build\physdump.obj : physdump.asm print.asm rdhigh.asm wrhigh.asm gdt.asm
	masm physdump.asm build\physdump.obj build\physdump.lst NUL

exe\physdump.exe : build\physdump.obj
	link build\physdump.obj;
	copy physdump.exe exe\.
	del physdump.exe



build\dosntfy.obj : dosntfy.asm print.asm rdhigh.asm wrhigh.asm gdt.asm
	masm dosntfy.asm build\dosntfy.obj build\dosntfy.lst NUL

exe\dosntfy.exe : build\dosntfy.obj
	link build\dosntfy.obj;
	copy dosntfy.exe exe\.
	del dosntfy.exe



build\tgdrv.obj : tgdrv.asm
	masm tgdrv.asm build\tgdrv.obj build\tgdrv.lst NUL

exe\tgdrv.com : build\tgdrv.obj
	link build\tgdrv.obj;
	exe2bin tgdrv.exe tgdrv.com
	copy tgdrv.com exe\.
	del tgdrv.exe
	del tgdrv.com



build\tgdrvsys.obj : tgdrvsys.asm
	masm tgdrvsys.asm build\tgdrvsys.obj build\tgdrvsys.lst NUL

exe\tgdrv.sys : build\tgdrvsys.obj
	link build\tgdrvsys.obj;
	exe2bin tgdrvsys.exe tgdrv.sys
	copy tgdrv.sys exe\.
	del tgdrvsys.exe
	del tgdrv.sys
