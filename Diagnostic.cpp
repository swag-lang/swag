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
        SWAG_ASSERT(sourceFile);
        fs::path path = sourceFile->path;
        g_Log.print(path.string().c_str());
        if (hasRangeLocation)
            g_Log.print(format(":%d:%d:%d:%d: ", startLocation.line + 1, startLocation.column + 1, endLocation.line + 1, endLocation.column + 1));
        else if (hasLocation)
            g_Log.print(format(":%d:%d:%d:%d: ", startLocation.line + 1, startLocation.column + 1, startLocation.line + 1, startLocation.column + 1));
        else
            g_Log.print(": ");
    }

    // User message
    g_Log.print(textMsg);
    g_Log.eol();

    // Code comment
    if (!codeComment.empty())
    {
        if (!verboseMode)
            g_Log.setColor(LogColor::DarkYellow);
        g_Log.print("remark: ");
        g_Log.print(codeComment);
        g_Log.eol();
    }

    if (!verboseMode)
        g_Log.setColor(LogColor::Cyan);

    // Source code
    if (hasFile && hasLocation && printSource && g_CommandLine.errorSourceOut)
    {
        // Get all lines of code
        vector<Utf8> lines;
        if (showMultipleCodeLines)
        {
            for (int i = -2; i <= 0; i++)
            {
                if (startLocation.line + i < 0)
                    continue;
                lines.push_back(sourceFile->getLine(startLocation.line + i));
            }
        }
        else
        {
            lines.push_back(sourceFile->getLine(startLocation.line));
        }

        // Print all lines
        for (auto& line : lines)
        {
            const char* pz = line.c_str();
            if (*pz && *pz != '\n' && *pz != '\r')
            {
                g_Log.print("   >  ");
                g_Log.print(line);
                g_Log.eol();
            }
        }

        // Show "^^^^^^^"
        if (showRange)
        {
            for (int i = 0; i < 6; i++)
                g_Log.print(" ");

            auto backLine = lines.back();
            for (int i = 0; i < startLocation.column; i++)
            {
                if (backLine[i] == '\t')
                    g_Log.print("\t");
                else
                    g_Log.print(" ");
            }

            int range = 1;
            if (!hasRangeLocation)
                range = 1;
            else if (endLocation.line == startLocation.line)
                range = endLocation.column - startLocation.column;
            else
                range = (int) lines.back().length() - startLocation.column;
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
