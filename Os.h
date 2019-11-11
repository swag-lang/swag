#pragma once
enum class LogColor;
enum class BackendOutputType;
#include "Log.h"

#ifdef WIN32
#include "OsWin32.h"
#endif

namespace OS
{
    extern void          setup();
    extern void          consoleSetup();
    extern void          consoleSetColor(LogColor color);
    extern bool          doProcess(const string& cmdline, const string& currentDirectory, bool logAll, uint32_t& numErrors, LogColor logColor = LogColor::DarkCyan, const char* logPrefix = nullptr);
    extern string        getOutputFileExtension(BackendOutputType type);
    extern string        getLastErrorAsString();
    extern swag_tls_id_t tlsAlloc();
    extern void          tlsSetValue(swag_tls_id_t id, void* value);
    extern void*         tlsGetValue(swag_tls_id_t id);
} // namespace OS
