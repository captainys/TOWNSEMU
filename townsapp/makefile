build\hdread.obj : hdimage\hdread.c
	hcd386 hdimage\hdread.c -obj build\hdread.obj

exp\HDREAD.EXP : build\hdread.obj
	386link build\hdread.obj @libs.LN -exe exp\HDREAD.EXP



build\tfrio.obj : filetfr\TFRIO.ASM
	386asm filetfr\TFRIO.ASM -OBJECT build\tfrio.OBJ

build\ftclient.obj : filetfr\FTCLIENT.C
	hcd386 filetfr\FTCLIENT.C -obj build\ftclient.obj

exp\ftclient.exp : build\tfrio.obj build\ftclient.obj filetfr\FTCLIENT.LN
	386link @filetfr\FTCLIENT.LN -exe exp\FTCLIENT.EXP
