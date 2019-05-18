#pragma
class Global
{
public:
    fs::path exePath;
    int      numCores = 0;

    Global();
};

extern class Global        g_Global;
extern struct CommandLine  g_CommandLine;
extern class Log           g_Log;
extern class Error         g_Error;
extern struct Stats        g_Stats;
extern class ThreadManager g_ThreadMgr;
extern class LanguageSpec  g_LangSpec;
extern struct PoolFactory  g_Pool;

extern wstring format(const wchar_t* format, ...);

#define SWAG_CHECK(__expr) { if (!(__expr)) return false; }