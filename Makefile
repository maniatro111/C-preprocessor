build: hash_table.obj every_define.obj main.obj
	cl /MD $** /Feso-cpp.exe
	
hash_table.obj: hash_table.c
	cl /MD /c $** /Fo$@
	
every_define.obj: every_define.c
	cl /MD /c $** /Fo$@
	
main.obj: main.c
	cl /MD /c $** /Fo$@

clean:
	del *.obj so-cpp.exe
