#pragma once
#include "SourceFile.h"
#include "Tokenizer.h"

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
    Diagnostic(SourceFile* file, const SourceLocation& start, const SourceLocation& end, const utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
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

    Diagnostic(SourceFile* file, const SourceLocation& start, const utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
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

    Diagnostic(SourceFile* file, const Token& token, const utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : m_file{file}
        , m_startLocation{token.startLocation}
        , m_endLocation{token.endLocation}
        , m_msg{msg}
        , m_level{level}
        , m_hasFile{true}
        , m_hasLocation{true}
        , m_hasRangeLocation{true}
        , m_printSource{true}
    {
    }

	Diagnostic(SourceFile* file, const utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : m_file{file}
        , m_msg{msg}
        , m_level{level}
        , m_hasFile{true}
        , m_hasLocation{false}
    {
    }

    SourceFile*     m_file = nullptr;
    SourceLocation  m_startLocation;
    SourceLocation  m_endLocation;
	utf8			m_msg;
    DiagnosticLevel m_level;
    bool            m_hasFile          = false;
    bool            m_hasLocation      = false;
    bool            m_hasRangeLocation = false;
    bool            m_printSource      = false;

    void report(bool verboseMode = false) const;
};
