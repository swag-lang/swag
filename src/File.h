#pragma once
#include "Utf8.h"

bool openFile(FILE** fileHandle, const char* path, const char* mode);
void closeFile(FILE** fileHandle);
