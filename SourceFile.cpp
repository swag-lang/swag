#include "pch.h"
#include "SourceFile.h"
#include "Diagnostic.h"
#include "Workspace.h"
#include "Stats.h"
#include "DiagnosticInfos.h"

const auto BUF_SIZE = 2048;

SourceFile::SourceFile()
{
    buffer = new char[BUF_SIZE];
    cleanCache();
}

void SourceFile::cleanCache()
{
    bufferSize    = 0;
    bufferCurSeek = 0;
    fileSeek      = 0;
    doneLoading   = false;
}

bool SourceFile::checkFormat()
{
    // Read header
    uint8_t c1 = buffer[0];
    uint8_t c2 = buffer[1];
    uint8_t c3 = buffer[2];
    uint8_t c4 = buffer[3];

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

void SourceFile::seekTo(long seek)
{
    fseek(fileHandle, seek, SEEK_SET);
}

long SourceFile::readTo()
{
    return (long) fread(buffer, 1, BUF_SIZE, fileHandle);
}

void SourceFile::loadRequest()
{
    buffer[0] = 0;

    LoadRequest req;
    req.seek       = fileSeek;
    req.loadedSize = 0;
    load(&req);

    bufferSize  = req.loadedSize;
    doneLoading = req.loadedSize != BUF_SIZE ? true : false;
    totalRead += req.loadedSize;
    fileSeek += bufferSize;

    if (doneLoading)
        close();
}

char SourceFile::getPrivateChar()
{
    if (externalBuffer)
    {
        if (seekExternal >= externalSize)
        {
            doneLoading = true;
            return 0;
        }

        return externalBuffer[seekExternal++];
    }

    if (directMode)
    {
        if (!fileHandle)
            return 0;
        auto c = fgetc(fileHandle);
        return c == EOF ? 0 : (char) c;
    }

    if (bufferCurSeek >= bufferSize)
    {
        // Done
        if (doneLoading)
        {
            close();
            return 0;
        }

        // First time, open and read in sync. This is faster to open files in jobs that letting
        // the loading thread open files one by one
        if (!openedOnce)
        {
            if (!openRead())
                return 0;
            bufferSize = readTo();
            if (!checkFormat())
                return false;
            fileSeek = BUF_SIZE;
        }
        else
        {
            loadRequest();
            bufferCurSeek = 0;
        }

        // Be sure there's something in the current buffer
        if (bufferCurSeek >= bufferSize)
        {
            close();
            return 0;
        }
    }

    char c = buffer[bufferCurSeek++];
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

Utf8 SourceFile::getLine(long seek)
{
    scoped_lock lk(mutexGetLine);

    if (!externalBuffer)
    {
        openRead();
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
        line += c;
        column++;
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

    //::MessageBoxA(NULL, "", "", 0);

    // Raise error
    g_Workspace.numErrors++;

    // Print error
    diag.report();
    if (g_CommandLine.errorNoteOut)
    {
        for (auto note : notes)
            note->report();
    }

    if (diag.showDiagnosticInfos)
        g_diagnosticInfos.log();
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

void SourceFile::load(LoadRequest* request)
{
    request->loadedSize = 0;
    if (openRead())
    {
        seekTo(request->seek);
        request->loadedSize = readTo();
    }

    // Stored in a static to avoid polling the function (there's a potential crt lock there)
    static int maxStdIo = 0;
    if (maxStdIo == 0)
        maxStdIo = _getmaxstdio();

    if (g_Stats.maxOpenFiles > maxStdIo / 2)
        close();
}
