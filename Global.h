#pragma 
class Global
{
public:
	fs::path exePath;
	int numCores = 0;

	Global();
};

extern class Global			g_Global;
extern struct CommandLine	g_CommandLine;
extern class Log			g_Log;
extern class Error			g_Error;