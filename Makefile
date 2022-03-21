build: hash_map.obj functions.obj main.obj
	cl /W3 /MD /D_CRT_SECURE_NO_DEPRECATE $** /Feso-cpp.exe
	
hash_map.obj: hash_map.c
	cl /W3 /MD /D_CRT_SECURE_NO_DEPRECATE /c $** /Fo$@
	
functions.obj: functions.c
	cl /W3 /MD /D_CRT_SECURE_NO_DEPRECATE /c $** /Fo$@
	
main.obj: main.c
	cl /W3 /MD /D_CRT_SECURE_NO_DEPRECATE /c $** /Fo$@

clean:
	del *.obj so-cpp.exe
