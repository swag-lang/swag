#include "pch.h"
#include "Diagnostic.h"
#include "Global.h"
#include "Log.h"
#include "CommandLine.h"

void Diagnostic::defaultColor(bool verboseMode) const
{
    if (verboseMode)
        g_Log.setColor(LogColor::DarkCyan);
    else
        g_Log.setColor(LogColor::White);
}

void Diagnostic::report(bool verboseMode) const
{
	defaultColor(verboseMode);

    // Source file and location
    if (hasFile)
    {
        g_Log.print(sourceFile->path.filename().string().c_str());
        if (hasLocation)
            g_Log.print(format(":%d:%d: ", startLocation.line + 1, startLocation.column + 1));
        else
            g_Log.print(": ");
    }

    // Message level
    switch (errorLevel)
    {
    case DiagnosticLevel::Error:
        if (!verboseMode)
            g_Log.setColor(LogColor::Red);
        g_Log.print("error: ");
		break;
    case DiagnosticLevel::Warning:
        if (!verboseMode)
            g_Log.setColor(LogColor::Magenta);
        g_Log.print("warning: ");
        break;
    case DiagnosticLevel::Note:
        if (!verboseMode)
            g_Log.setColor(LogColor::White);
        g_Log.print("note: ");
        break;
    }

    // User message
	defaultColor(verboseMode);
	g_Log.print(textMsg);
    g_Log.eol();

    // Source code
    if (hasFile && hasLocation && printSource)
    {
        // Remove blanks at the start of the source line
        auto        tmpLine = sourceFile->getLine(startLocation.seekStartLine);
        int         offset  = 0;
        const char* buf     = tmpLine.c_str();
        while (*buf == ' ')
        {
            buf++;
            offset++;
        }

        // Indent
        Utf8 line = "  ";
        line += buf;
        offset -= 2;

		if (!verboseMode)
			g_Log.setColor(LogColor::Gray);
        g_Log.print(line);
        g_Log.eol();

        for (int i = 0; i < startLocation.column - offset; i++)
            g_Log.print(" ");

        int range = 1;
        if (!hasRangeLocation)
            range = 1;
        else if (endLocation.line == startLocation.line)
            range = endLocation.column - startLocation.column;
        else
            range = (int) line.length() - startLocation.column - offset;
        range = max(1, range);
        if (!verboseMode)
            g_Log.setColor(LogColor::Green);
        for (int i = 0; i < range; i++)
            g_Log.print("^");
        g_Log.eol();
    }

    g_Log.setDefaultColor();
}