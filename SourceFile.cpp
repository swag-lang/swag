#include "pch.h"
#include "SourceFile.h"
#include "ThreadManager.h"
#include "LoadingThread.h"
#include "CommandLine.h"
#include "Global.h"
#include "Stats.h"
#include "Log.h"
#include "Diagnostic.h"
#include "Module.h"
#include "Workspace.h"
#include "PoolFactory.h"

SourceFile::SourceFile()
{
    m_poolFactory = new PoolFactory;

    const auto BUF_SIZE = 4096;
    m_bufferSize        = BUF_SIZE;
    m_buffers[0]        = new char[m_bufferSize];
    m_buffers[1]        = new char[m_bufferSize];

    cleanCache();
}

void SourceFile::cleanCache()
{
    m_requests[0]    = nullptr;
    m_requests[1]    = nullptr;
    m_buffersSize[0] = 0;
    m_buffersSize[1] = 0;
    m_bufferCurSeek  = 0;
    m_bufferCurIndex = 0;
    m_fileSeek       = 0;
    m_doneLoading    = false;
}

bool SourceFile::open()
{
    if (m_file != nullptr)
        return true;

    auto err = _wfopen_s(&m_file, m_path.c_str(), L"rb");
    if (m_file == nullptr)
    {
        char buf[256];
        strerror_s(buf, err);
        report({this, format("error reading file: '%s'", buf)});
        return false;
    }

    setvbuf(m_file, nullptr, _IONBF, 0);
    return true;
}

bool SourceFile::ensureOpen()
{
    if (!m_file && !m_openedOnce)
    {
        m_openedOnce = true;
        if (!open())
            return false;
        if (!checkFormat())
            return false;
    }
    else if (!m_file)
        open();
    return true;
}

bool SourceFile::checkFormat()
{
    // Read header
    auto c1 = fgetc(m_file);
    auto c2 = fgetc(m_file);
    auto c3 = fgetc(m_file);
    auto c4 = fgetc(m_file);

    if (c1 == 0xEF && c2 == 0xBB && c3 == 0xBF)
    {
        m_textFormat = TextFormat::UTF8;
        m_fileSeek   = 3;
        m_headerSize = 3;
        return true;
    }

    if ((c1 == 0xFE && c2 == 0xFF)                                // UTF-16 BigEndian
        || (c1 == 0xFF && c2 == 0xFE)                             // UTF-16 LittleEndian
        || (c1 == 0x00 && c2 == 0x00 && c3 == 0xFE && c4 == 0xFF) // UTF-32 BigEndian
        || (c1 == 0xFF && c2 == 0xFE && c3 == 0x00 && c4 == 0x00) // UTF-32 BigEndian
        || (c1 == 0x0E && c2 == 0xFE && c3 == 0xFF)               // SCSU
        || (c1 == 0xDD && c2 == 0x73 && c3 == 0x66 && c4 == 0x73) // UTF-EBCDIC
        || (c1 == 0x2B && c2 == 0x2F && c3 == 0x76 && c4 == 0x38) // UTF-7
        || (c1 == 0x2B && c2 == 0x2F && c3 == 0x76 && c4 == 0x39) // UTF-7
        || (c1 == 0x2B && c2 == 0x2F && c3 == 0x76 && c4 == 0x2B) // UTF-7
        || (c1 == 0x2B && c2 == 0x2F && c3 == 0x76 && c4 == 0x2F) // UTF-7
        || (c1 == 0xFB && c2 == 0xEE && c3 == 0x28)               // BOCU-1
        || (c1 == 0xF7 && c2 == 0x64 && c3 == 0x4C)               // UTF-1 BigEndian
        || (c1 == 0x84 && c2 == 0x31 && c3 == 0x95 && c4 == 0x33) // GB-18030
    )
    {
        report({this, "invalid file format, should be ascii, utf-8 or utf-8-bom"});
        return false;
    }

    return true;
}

void SourceFile::close()
{
    if (m_file)
    {
        fclose(m_file);
        m_file = nullptr;
    }
}

void SourceFile::seekTo(long seek)
{
    fseek(m_file, seek, SEEK_SET);
}

long SourceFile::readTo(char* buffer)
{
    return (long) fread(buffer, 1, m_bufferSize, m_file);
}

void SourceFile::waitRequest(int reqNum)
{
    std::unique_lock<std::mutex> lk(m_mutexNotify);
    if (!m_requests[reqNum]->done)
        m_Cv.wait(lk);
}

void SourceFile::notifyLoad()
{
    std::unique_lock<std::mutex> lk(m_mutexNotify);
    m_Cv.notify_one();
}

void SourceFile::validateRequest(int reqNum)
{
    auto loadingTh = g_ThreadMgr.m_loadingThread;
    auto req       = m_requests[reqNum];

    m_buffersSize[reqNum] = req->loadedSize;
    m_doneLoading         = req->loadedSize != m_bufferSize ? true : false;
    m_totalRead += req->loadedSize;

    loadingTh->releaseRequest(req);
    m_requests[reqNum] = nullptr;
    if (m_doneLoading)
        close();
}

void SourceFile::buildRequest(int reqNum)
{
    auto loadingTh = g_ThreadMgr.m_loadingThread;
    auto req       = loadingTh->newRequest();

    req->file          = this;
    req->seek          = m_fileSeek;
    req->buffer        = m_buffers[reqNum];
    req->buffer[0]     = 0;
    m_requests[reqNum] = req;

    loadingTh->addRequest(req);
    m_fileSeek += m_bufferSize;
}

char SourceFile::getPrivateChar()
{
    if (m_directMode)
    {
        if (!m_file)
            return 0;
        auto c = fgetc(m_file);
        return c == EOF ? 0 : (char) c;
    }

    if (m_bufferCurSeek >= m_buffersSize[m_bufferCurIndex])
    {
        if (!m_openedOnce && !ensureOpen())
            return 0;

        // Done
        if (m_doneLoading)
        {
            close();
            return 0;
        }

        auto nextBufIndex = (m_bufferCurIndex + 1) % 2;

        if (!m_requests[nextBufIndex])
            buildRequest(nextBufIndex);
        waitRequest(nextBufIndex);
        validateRequest(nextBufIndex);

        m_bufferCurIndex = (m_bufferCurIndex + 1) % 2;
        m_bufferCurSeek  = 0;

        // Make an async request to read the next buffer
        if (!m_doneLoading)
        {
            nextBufIndex = (m_bufferCurIndex + 1) % 2;
            buildRequest(nextBufIndex);
        }
    }

    char c = m_buffers[m_bufferCurIndex][m_bufferCurSeek++];
    return c;
}

char32_t SourceFile::getChar(unsigned& offset)
{
    char c = getPrivateChar();
    offset = 1;

    // utf8
    if (m_textFormat == TextFormat::UTF8)
    {
        if ((c & 0x80) == 0)
            return c;

        char32_t wc;
        if ((c & 0xE0) == 0xC0)
        {
            wc = (c & 0x1F) << 6;
            wc |= (getPrivateChar() & 0x3F);
            offset += 1;
            return wc;
        }

        if ((c & 0xF0) == 0xE0)
        {
            wc = (c & 0xF) << 12;
            wc |= (getPrivateChar() & 0x3F) << 6;
            wc |= (getPrivateChar() & 0x3F);
            offset += 2;
            return wc;
        }

        if ((c & 0xF8) == 0xF0)
        {
            wc = (c & 0x7) << 18;
            wc |= (getPrivateChar() & 0x3F) << 12;
            wc |= (getPrivateChar() & 0x3F) << 6;
            wc |= (getPrivateChar() & 0x3F);
            offset += 3;
            return wc;
        }

        if ((c & 0xFC) == 0xF8)
        {
            wc = (c & 0x3) << 24;
            wc |= (c & 0x3F) << 18;
            wc |= (c & 0x3F) << 12;
            wc |= (c & 0x3F) << 6;
            wc |= (c & 0x3F);
            return wc;
        }

        if ((c & 0xFE) == 0xFC)
        {
            wc = (c & 0x1) << 30;
            wc |= (c & 0x3F) << 24;
            wc |= (c & 0x3F) << 18;
            wc |= (c & 0x3F) << 12;
            wc |= (c & 0x3F) << 6;
            wc |= (c & 0x3F);
            return wc;
        }
    }

    return '?';
}

void SourceFile::waitEndRequests()
{
    while (m_requests[m_bufferCurIndex] && !m_requests[m_bufferCurIndex]->done) {}
    while (m_requests[(m_bufferCurIndex + 1) % 2] && !m_requests[(m_bufferCurIndex + 1) % 2]->done) {}
}

utf8 SourceFile::getLine(long seek)
{
    // Be sure there's no pending requests
    waitEndRequests();

    open();
    seekTo(seek + m_headerSize);
    m_directMode = true;

    utf8 line;
    int  column = 0;
    while (true)
    {
        unsigned offset = 0;
        auto     c      = getChar(offset);
        if (!c || c == '\n')
            break;
        if (c == '\t')
        {
            column++;
            line += " ";
            while (column % g_CommandLine.tabSize)
            {
                column++;
                line += " ";
            }
        }
        else
            line += c;
    }

    m_directMode = false;
    close();
    return line;
}

void SourceFile::report(const Diagnostic& diag)
{
    if (m_silent > 0)
        return;

    // Do not raise an error if we are waiting for one, during tests
    if (m_unittestError && diag.m_level == DiagnosticLevel::Error)
    {
        m_unittestError--;
        if (g_CommandLine.verbose)
        {
            g_Log.lock();
            diag.report(true);
            g_Log.unlock();
        }

        return;
    }

    // Raise error
    g_Workspace.numErrors++;
    m_module->numErrors++;
    g_Log.lock();
    diag.report();
    g_Log.unlock();
}