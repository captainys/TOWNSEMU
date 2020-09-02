build\hdread.obj : hdimage\hdread.c
	hcd386 hdimage\hdread.c -obj build\hdread.obj

build\hdread.exp : build\hdread.obj
	386link build\hdread.obj @libs.LN -exe build\hdread.exp
