alltargets : exe\physdump.exe exe\dosntfy.exe



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
