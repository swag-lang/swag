#pragma once
enum class LogColor;
enum class BackendOutputType;
#include "Log.h"

namespace OS
{
    extern void   setup();
    extern void   consoleSetup();
    extern void   consoleSetColor(LogColor color);
    extern bool   doProcess(const string& cmdline, const string& currentDirectory, bool logAll, uint32_t& numErrors, LogColor logColor = LogColor::DarkCyan);
    extern string getOutputFileExtension(BackendOutputType type);
} // namespace OS
