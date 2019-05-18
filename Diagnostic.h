#pragma once
#include "SourceFile.h"

enum DiagnosticLevel
{
    Error,
    Warning,
    Verbose,
    Message,
};

class Diagnostic
{
public:
    Diagnostic(SourceFile* file, const SourceLocation& start, const SourceLocation& end, const wstring& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : m_file{file}
        , m_startLocation{start}
        , m_endLocation{end}
        , m_msg{msg}
        , m_level{level}
        , m_hasFile{true}
        , m_hasLocation{true}
        , m_hasRangeLocation{true}
        , m_printSource{true}
    {
    }

    Diagnostic(SourceFile* file, const SourceLocation& start, const wstring& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : m_file{file}
        , m_startLocation{start}
        , m_msg{msg}
        , m_level{level}
        , m_hasFile{true}
        , m_hasLocation{true}
        , m_hasRangeLocation{false}
        , m_printSource{true}
    {
    }

    SourceFile*     m_file = nullptr;
    SourceLocation  m_startLocation;
    SourceLocation  m_endLocation;
    wstring         m_msg;
    DiagnosticLevel m_level;
    bool            m_hasFile = false;
    bool            m_hasLocation = false;
    bool            m_hasRangeLocation = false;
    bool            m_printSource = false;

    void report(bool verboseMode = false);
};
