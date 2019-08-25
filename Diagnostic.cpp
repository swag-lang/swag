#include "pch.h"
#include "Diagnostic.h"
#include "Log.h"
#include "SourceFile.h"

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
        g_Log.print(sourceFile->path.filename().string().c_str());
        if (hasLocation)
            g_Log.print(format(":%d:%d: ", startLocation.line + 1, startLocation.column + 1));
        else
            g_Log.print(": ");
    }

    // User message
    g_Log.print(textMsg);
    g_Log.eol();
    if (!verboseMode)
        g_Log.setColor(LogColor::Gray);

    // Source code
    if (hasFile && hasLocation && printSource && g_CommandLine.errorSourceOut)
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

        g_Log.print(line);
        g_Log.eol();

        if (showRange)
        {
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
    }

    g_Log.setDefaultColor();
}

bool ErrorContext::report(const Diagnostic& diag, const Diagnostic* note, const Diagnostic* note1)
{
    vector<const Diagnostic*> notes;
    if (note)
        notes.push_back(note);
    if (note1)
        notes.push_back(note1);
    return report(diag, notes);
}

bool ErrorContext::report(const Diagnostic& diag, const vector<const Diagnostic*>& notes)
{
    auto copyNotes = notes;

    if (genericInstanceTree.size())
    {
        auto       first = genericInstanceTree[0];
        Diagnostic note{genericInstanceTreeFile[0], first, format("occured during generic instantiation of '%s'", first->name.c_str()), DiagnosticLevel::Note};
        copyNotes.push_back(&note);
        return sourceFile->report(diag, copyNotes);
    }

    return sourceFile->report(diag, copyNotes);
}