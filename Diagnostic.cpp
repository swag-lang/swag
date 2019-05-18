#include "pch.h"
#include "Diagnostic.h"
#include "Global.h"
#include "SourceFile.h"
#include "Log.h"

void Diagnostic::report(bool verboseMode)
{
    if (verboseMode)
        g_Log.setColor(LogColor::DarkCyan);

    // Source file and location
    if (m_hasFile)
    {
        g_Log.print(m_file->m_path.filename());
        if (m_hasLocation)
            g_Log.print(format(L":%d:%d: ", m_startLocation.line + 1, m_startLocation.column + 1));
    }

    // Message level
    switch (m_level)
    {
    case DiagnosticLevel::Error:
        if (!verboseMode)
            g_Log.setColor(LogColor::Red);
        g_Log.print(L"error: ");
        if (!verboseMode)
            g_Log.setDefaultColor();
        break;
    case DiagnosticLevel::Warning:
        if (!verboseMode)
            g_Log.setColor(LogColor::Magenta);
        g_Log.print(L"warning: ");
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
        auto line   = m_file->getLine(m_startLocation.seekStartLine);
        int  offset = 0;
        while (line[0] == ' ')
        {
            line = line.substr(1);
            offset++;
        }

		// Indent
		line = L"  " + line;
        offset -= 2;

        g_Log.print(line);
        g_Log.eol();

        for (int i = 0; i < m_startLocation.column - offset; i++)
            g_Log.print(L" ");

		int endColumn;
        if (!m_hasRangeLocation)
            endColumn = m_startLocation.column - offset + 1;
        else if (m_endLocation.line == m_startLocation.line)
            endColumn = m_endLocation.column - offset;
        else
            endColumn = (int) line.length() - m_startLocation.column - offset;
        for (int i = m_startLocation.column - offset; i < endColumn; i++)
            g_Log.print(L"^");
        g_Log.eol();
    }
}