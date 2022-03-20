build: hash_table.obj functions.obj main.obj
	cl /W3 /MD /D_CRT_SECURE_NO_DEPRECATE $** /Feso-cpp.exe
	
hash_table.obj: hash_table.c
	cl /W3 /MD /D_CRT_SECURE_NO_DEPRECATE /c $** /Fo$@
	
functions.obj: functions.c
	cl /W3 /MD /D_CRT_SECURE_NO_DEPRECATE /c $** /Fo$@
	
main.obj: main.c
	cl /W3 /MD /D_CRT_SECURE_NO_DEPRECATE /c $** /Fo$@

clean:
	del *.obj so-cpp.exe
