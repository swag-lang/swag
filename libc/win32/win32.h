#pragma once

#define STD_OUTPUT_HANDLE -11

EXTERN_C void*        GetStdHandle(int);
EXTERN_C int          WriteFile(void*, const void*, int, const int*, void*);
EXTERN_C void         RaiseException(int, int, int, const void*);
EXTERN_C void*        LoadLibraryA(const char*);
EXTERN_C int          TlsAlloc();
EXTERN_C void         TlsSetValue(int, void*);
EXTERN_C void*        TlsGetValue(int);
EXTERN_C const char*  GetCommandLineA();
EXTERN_C void         ExitProcess(unsigned int);
EXTERN_C unsigned int MessageBoxA(void*, const char*, const char*, unsigned int);
EXTERN_C void         DebugBreak();

#define MB_ICONHAND 0x00000010
#define MB_CANCELTRYCONTINUE 0x00000006
#define IDCANCEL 2
#define IDTRYAGAIN 10
#define IDCONTINUE 11
