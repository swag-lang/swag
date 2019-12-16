#include "pch.h"
#include "Diagnostic.h"
#include "Log.h"
#include "SourceFile.h"
#include "SymTable.h"

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

    // Source file and location
    if (hasFile)
    {
        fs::path path = sourceFile->path;
        g_Log.print(path.filename().string().c_str());
        if (hasLocation)
            g_Log.print(format(":%d:%d: ", startLocation.line + 1, startLocation.column + 1));
        else
            g_Log.print(": ");
    }

    // User message
    g_Log.print(textMsg);
    g_Log.eol();
    if (!verboseMode)
        g_Log.setColor(LogColor::Cyan);

    // Source code
    if (hasFile && hasLocation && printSource && g_CommandLine.errorSourceOut)
    {
        // Get all lines of code
        vector<Utf8> lines;
        for (int i = 0; i < REPORT_NUM_CODE_LINES; i++)
        {
            if (startLocation.seekStartLine[i] == -1)
                continue;
            if (!showRange && i != REPORT_NUM_CODE_LINES - 1)
                continue;
            lines.push_back(sourceFile->getLine(startLocation.seekStartLine[i]));
        }

        // Compute the minimal number of blanks at the start (to unindent)
        uint32_t minOffset = UINT32_MAX;
        for (auto& line : lines)
        {
            const char* buf = line.c_str();

            uint32_t offset = 0;
            while (*buf == ' ')
            {
                buf++;
                offset++;
            }

            minOffset = min(minOffset, offset);
        }

        // Print all lines
        //g_Log.eol();
        for (auto& line : lines)
        {
            const char* pz = line.c_str() + minOffset;
            if (*pz && *pz != '\n' && *pz != '\r')
            {
                g_Log.print("   >  ");
                Utf8 tmp(line.c_str() + minOffset);
                g_Log.print(tmp);
                g_Log.eol();
            }
        }

        if (showRange)
        {
            minOffset -= 6;
            for (int i = 0; i < startLocation.column - (int) minOffset; i++)
                g_Log.print(" ");

            int range = 1;
            if (!hasRangeLocation)
                range = 1;
            else if (endLocation.line == startLocation.line)
                range = endLocation.column - startLocation.column;
            else
                range = (int) lines.back().length() - startLocation.column - minOffset;
            range = max(1, range);

            if (!verboseMode)
            {
                switch (errorLevel)
                {
                case DiagnosticLevel::Error:
                    g_Log.setColor(LogColor::Red);
                    break;
                case DiagnosticLevel::Warning:
                    g_Log.setColor(LogColor::Magenta);
                    break;
                case DiagnosticLevel::Note:
                    g_Log.setColor(LogColor::Blue);
                    break;
                }
            }

            for (int i = 0; i < range; i++)
                g_Log.print("^");
            g_Log.eol();
        }

        g_Log.eol();
    }

    g_Log.setDefaultColor();
}
