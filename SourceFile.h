#pragma once
#include "Pool.h"

enum ErrorIO
{
    Ok,
    BadFormat,
};

enum TextFormat
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

class SourceFile : public PoolElement
{
    friend class LoadingThread;

public:
    fs::path m_path;

    int  m_unittestError = 0;
    bool m_doLex        = true;

public:
    ~SourceFile();
    void     construct() override;
    void     reset() override;
    unsigned getChar();
    wstring  getLine(long seek);
    void     report(const class Diagnostic& diag);

private:
    void open();
    void cleanCache();
    void seekTo(long seek);
    long readTo(char* buffer);
    void notifyLoad();
    void close();
    void waitRequest(int reqNum);
    void validateRequest(int reqNum);
    void buildRequest(int reqNum);
    char getPrivateChar();

private:
    ErrorIO                      m_errorIO    = ErrorIO::Ok;
    TextFormat                   m_textFormat = TextFormat::UTF8;
    int                          m_bufferSize;
    FILE*                        m_file;
    long                         m_fileSeek       = 0;
    long                         m_bufferCurSeek  = 0;
    int                          m_bufferCurIndex = 0;
    char*                        m_buffers[2];
    struct LoadingThreadRequest* m_requests[2];
    long                         m_buffersSize[2];
    mutex                        m_mutexNotify;
    bool                         m_doneLoading = false;
    int                          m_totalRead   = 0;
    condition_variable           m_Cv;
};
