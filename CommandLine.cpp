#include "pch.h"
#include "CommandLine.h"
#include "Global.h"

bool CommandLine::process(int argc, const char *argv[])
{
	for (int i = 0; i < argc; i++)
	{
		if (!strcmp(argv[i], "--verbose"))
			g_CommandLine.verbose = true;
	}

	return true;
}
