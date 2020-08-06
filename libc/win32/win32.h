#pragma once

#define STD_OUTPUT_HANDLE -11

EXTERN_C void* GetStdHandle(int);
EXTERN_C int   WriteFile(void*, const void*, int, const int*, void*);
EXTERN_C void  RaiseException(int, int, int, const void*);
EXTERN_C void* LoadLibraryA(const char*);
EXTERN_C int   TlsAlloc();
EXTERN_C void  TlsSetValue(int, void*);
EXTERN_C void* TlsGetValue(int);
