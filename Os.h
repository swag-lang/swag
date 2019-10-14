#pragma once
enum class LogColor;

namespace OS
{
    extern void setup();
    extern void consoleSetup();
    extern void consoleSetColor(LogColor color);
    extern void consoleSetDefaultColor();
    extern bool doProcess(const string& cmdline, const string& compilerPath, bool logAll, uint32_t& numErrors);
} // namespace OS
