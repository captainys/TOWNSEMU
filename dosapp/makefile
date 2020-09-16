build\physdump.obj : physdump.asm print.asm rdhigh.asm wrhigh.asm gdt.asm
	masm physdump.asm build\physdump.obj build\physdump.lst NUL

exe\physdump.exe : build\physdump.obj
	link build\physdump.obj;
	copy physdump.exe exe\.
	del physdump.exe
