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
#include "SymTable.h"

Pool<SourceFile> g_Pool_sourceFile;
const auto       BUF_SIZE = 512;

SourceFile::SourceFile()
    : bufferSize{BUF_SIZE}
{
    buffers[0] = new char[bufferSize];
    buffers[1] = new char[bufferSize];
    cleanCache();
}

void SourceFile::cleanCache()
{
    requests[0]    = nullptr;
    requests[1]    = nullptr;
    buffersSize[0] = 0;
    buffersSize[1] = 0;
    bufferCurSeek  = 0;
    bufferCurIndex = 0;
    fileSeek       = 0;
    doneLoading    = false;
}

bool SourceFile::open()
{
    if (fileHandle != nullptr)
        return true;
    openedOnce = true;

    auto err = _wfopen_s(&fileHandle, path.c_str(), L"rb");
    if (fileHandle == nullptr)
    {
        char buf[256];
        strerror_s(buf, err);
        report({this, format("error reading file: '%s'", buf)});
        return false;
    }

    setvbuf(fileHandle, nullptr, _IONBF, 0);
    return true;
}

bool SourceFile::checkFormat(int bufferIndex)
{
    // Read header
    uint8_t c1 = buffers[bufferIndex][0];
    uint8_t c2 = buffers[bufferIndex][1];
    uint8_t c3 = buffers[bufferIndex][2];
    uint8_t c4 = buffers[bufferIndex][3];

    if (c1 == 0xEF && c2 == 0xBB && c3 == 0xBF)
    {
        textFormat    = TextFormat::UTF8;
        bufferCurSeek = 3;
        headerSize    = 3;
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
    if (fileHandle)
    {
        fclose(fileHandle);
        fileHandle = nullptr;
    }
}

void SourceFile::seekTo(long seek)
{
    fseek(fileHandle, seek, SEEK_SET);
}

long SourceFile::readTo(char* buffer)
{
    return (long) fread(buffer, 1, bufferSize, fileHandle);
}

void SourceFile::waitRequest(int reqNum)
{
    std::unique_lock<std::mutex> lk(mutexNotify);
    if (!requests[reqNum]->done)
        condVar.wait(lk);
}

void SourceFile::notifyLoad()
{
    std::unique_lock<std::mutex> lk(mutexNotify);
    condVar.notify_one();
}

void SourceFile::validateRequest(int reqNum)
{
    auto loadingTh = g_ThreadMgr.loadingThread;
    auto req       = requests[reqNum];

    buffersSize[reqNum] = req->loadedSize;
    doneLoading         = req->loadedSize != bufferSize ? true : false;
    totalRead += req->loadedSize;

    loadingTh->releaseRequest(req);
    requests[reqNum] = nullptr;
    if (doneLoading)
        close();
}

void SourceFile::buildRequest(int reqNum)
{
    auto loadingTh = g_ThreadMgr.loadingThread;
    auto req       = loadingTh->newRequest();

    req->file        = this;
    req->seek        = fileSeek;
    req->buffer      = buffers[reqNum];
    req->buffer[0]   = 0;
    requests[reqNum] = req;

    loadingTh->addRequest(req);
    fileSeek += bufferSize;
}

char SourceFile::getPrivateChar()
{
    if (externalBuffer)
        return externalBuffer[seekExternal++];

    if (directMode)
    {
        if (!fileHandle)
            return 0;
        auto c = fgetc(fileHandle);
        return c == EOF ? 0 : (char) c;
    }

    if (bufferCurSeek >= buffersSize[bufferCurIndex])
    {
        // Done
        if (doneLoading)
        {
            close();
            return 0;
        }

        auto nextBufIndex = (bufferCurIndex + 1) % 2;

        // First time, open and read in sync. This is faster to open files in jobs that letting
        // the loading thread open files one by one
        if (!openedOnce)
        {
            if (!open())
                return 0;
            buffersSize[nextBufIndex] = readTo(buffers[nextBufIndex]);
            if (!checkFormat(nextBufIndex))
                return false;
            fileSeek = BUF_SIZE;
        }
        else
        {
            if (!requests[nextBufIndex])
                buildRequest(nextBufIndex);
            waitRequest(nextBufIndex);
            validateRequest(nextBufIndex);
            bufferCurSeek = 0;
        }

        bufferCurIndex = (bufferCurIndex + 1) % 2;

        // Make an async request to read the next buffer
        if (!doneLoading)
        {
            nextBufIndex = (bufferCurIndex + 1) % 2;
            buildRequest(nextBufIndex);
        }
    }

    char c = buffers[bufferCurIndex][bufferCurSeek++];
    return c;
}

char32_t SourceFile::getChar(unsigned& offset)
{
    char c = getPrivateChar();
    offset = 1;

    // utf8
    if (textFormat == TextFormat::UTF8)
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
    while (requests[bufferCurIndex] && !requests[bufferCurIndex]->done) {}
    while (requests[(bufferCurIndex + 1) % 2] && !requests[(bufferCurIndex + 1) % 2]->done) {}
}

Utf8 SourceFile::getLine(long seek)
{
    if (!externalBuffer)
    {
        waitEndRequests(); // Be sure there's no pending requests
        open();
        seekTo(seek + headerSize);
        directMode = true;
    }
    else
    {
        seekExternal = seek;
    }

    Utf8 line;
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
        {
            line += c;
            column++;
        }
    }

    directMode = false;
    close();
    return line;
}

bool SourceFile::report(const Diagnostic& diag, const vector<const Diagnostic*>& notes)
{
    if (silent > 0)
        return false;

    scoped_lock lock(g_Log.mutexAccess);
	numErrors++;
    module->numErrors++;

    // Do not raise an error if we are waiting for one, during tests
    if (unittestError && diag.errorLevel == DiagnosticLevel::Error)
    {
        unittestError--;
        if (g_CommandLine.verbose && g_CommandLine.verboseUnittestErrors)
        {
            diag.report(true);
            if (g_CommandLine.errorNoteOut)
            {
                for (auto note : notes)
                    note->report(true);
            }
        }

        return false;
    }

    // Raise error
    g_Workspace.numErrors++;

    // Print error
    diag.report();
    if (g_CommandLine.errorNoteOut)
    {
        for (auto note : notes)
            note->report();
    }

    return false;
}

bool SourceFile::report(const Diagnostic& diag, const Diagnostic* note, const Diagnostic* note1)
{
    vector<const Diagnostic*> notes;
    if (note)
        notes.push_back(note);
    if (note1)
        notes.push_back(note1);

    return report(diag, notes);
}
