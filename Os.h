#pragma once
enum class LogColor;
enum class BackendOutputType;

namespace OS
{
    extern void   setup();
    extern void   consoleSetup();
    extern void   consoleSetColor(LogColor color);
    extern void   consoleSetDefaultColor();
    extern bool   doProcess(const string& cmdline, const string& compilerPath, bool logAll, uint32_t& numErrors);
    extern string getOutputFileExtension(BackendOutputType type);
} // namespace OS
