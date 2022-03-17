build:
	cl /MD hash_table.c every_define.c main.c /Feso-cpp.exe

clean:
	del *.obj so-cpp.exe test3.out