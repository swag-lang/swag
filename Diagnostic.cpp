#include "pch.h"
#include "Diagnostic.h"
#include "Global.h"
#include "SourceFile.h"
#include "Log.h"

void Diagnostic::report(bool verboseMode) const
{
    if (verboseMode)
        g_Log.setColor(LogColor::DarkCyan);
    else
        g_Log.setDefaultColor();

    // Source file and location
    if (m_hasFile)
    {
		g_Log.print(m_file->m_path.filename().string().c_str());
        if (m_hasLocation)
            g_Log.print(format(":%d:%d: ", m_startLocation.line + 1, m_startLocation.column + 1));
        else
            g_Log.print(": ");
    }

    // Message level
    switch (m_level)
    {
    case DiagnosticLevel::Error:
        if (!verboseMode)
            g_Log.setColor(LogColor::Red);
        g_Log.print("error: ");
        if (!verboseMode)
            g_Log.setDefaultColor();
        break;
    case DiagnosticLevel::Warning:
        if (!verboseMode)
            g_Log.setColor(LogColor::Magenta);
        g_Log.print("warning: ");
        if (!verboseMode)
            g_Log.setDefaultColor();
        break;
    }

    // User message
    g_Log.print(m_msg);
    g_Log.eol();

    // Source code
    if (m_hasFile && m_hasLocation && m_printSource)
    {
        // Remove blanks at the start of the source line
        auto        tmpLine = m_file->getLine(m_startLocation.seekStartLine);
        int         offset  = 0;
        const char* buf     = tmpLine.c_str();
        while (*buf == ' ')
        {
            buf++;
            offset++;
        }

        // Indent
        utf8 line = "  ";
        line += buf;
        offset -= 2;

        g_Log.print(line);
        g_Log.eol();

        for (int i = 0; i < m_startLocation.column - offset; i++)
            g_Log.print(" ");

        int range = 1;
        if (!m_hasRangeLocation)
            range = 1;
        else if (m_endLocation.line == m_startLocation.line)
            range = m_endLocation.column - m_startLocation.column;
        else
            range = (int) line.length() - m_startLocation.column - offset;
        range = max(1, range);
        for (int i = 0; i < range; i++)
            g_Log.print("^");
        g_Log.eol();
    }

    g_Log.setDefaultColor();
}