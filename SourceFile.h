#pragma once
#include "Pool.h"
#include "Utf8.h"

enum class BuildPass
{
    Lexer,
    Syntax,
    Semantic,
    Full,
};

enum class TextFormat
{
    UTF8,
};

struct SourceLocation
{
    int line;
    int column;
    int seek;
    int seekStartLine;
};

struct SourceFile : public PoolElement
{
    SourceFile();
    char32_t getChar(unsigned& offset);
    utf8     getLine(long seek);
    void     report(const class Diagnostic& diag);

    bool open();
    bool ensureOpen();
    void cleanCache();
    void seekTo(long seek);
    long readTo(char* buffer);
    void notifyLoad();
    void close();
    void waitRequest(int reqNum);
    void validateRequest(int reqNum);
    void buildRequest(int reqNum);
    char getPrivateChar();
    void waitEndRequests();
    bool checkFormat();

    fs::path            m_path;
    BuildPass           m_buildPass     = BuildPass::Full;
    int                 m_unittestError = 0;
    int                 m_silent        = 0;
    struct Module*      m_module        = nullptr;
    struct AstNode*     m_astRoot       = nullptr;
    struct PoolFactory* poolFactory   = nullptr;

    TextFormat                   m_textFormat = TextFormat::UTF8;
    int                          m_bufferSize;
    int                          m_headerSize = 0;
    FILE*                        m_file;
    long                         m_fileSeek       = 0;
    long                         m_bufferCurSeek  = 0;
    int                          m_bufferCurIndex = 0;
    char*                        m_buffers[2];
    struct LoadingThreadRequest* m_requests[2];
    long                         m_buffersSize[2];
    mutex                        m_mutexNotify;
    bool                         m_doneLoading = false;
    bool                         m_directMode  = false;
    bool                         m_openedOnce  = false;
    int                          m_totalRead   = 0;
    condition_variable           m_Cv;
};
