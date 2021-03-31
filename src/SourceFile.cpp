#include "pch.h"
#include "SourceFile.h"
#include "Diagnostic.h"
#include "Workspace.h"
#include "Stats.h"
#include "Module.h"
#include "Os.h"
#include "Timer.h"
#include "ByteCodeStack.h"
#include "Context.h"
#include "Ast.h"

const auto BUF_SIZE = 2048;

SourceFile::SourceFile()
{
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
    if (!buffer)
        buffer = new char[BUF_SIZE];
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

void SourceFile::setExternalBuffer(char* buf, uint32_t size)
{
    buffer        = buf;
    bufferCurSeek = 0;
    bufferSize    = size;
    isExternal    = true;
}

char SourceFile::loadAndGetPrivateChar()
{
    // Done
    if (isExternal)
    {
        doneLoading = true;
        return 0;
    }

    if (doneLoading)
    {
        close();
        return 0;
    }

    // Read one chunk of file
    {
        Timer read(&g_Stats.readFilesTime);
        read.start();
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
        read.stop();
    }

    // Be sure there's something in the current buffer
    if (bufferCurSeek >= bufferSize)
    {
        if (!isExternal)
            delete buffer;
        close();
        return 0;
    }

    return buffer[bufferCurSeek++];
}

char SourceFile::getPrivateChar()
{
    if (bufferCurSeek >= bufferSize)
        return loadAndGetPrivateChar();
    return buffer[bufferCurSeek++];
}

uint32_t SourceFile::getChar(unsigned& offset)
{
    char c = bufferCurSeek >= bufferSize ? loadAndGetPrivateChar() : buffer[bufferCurSeek++];
    offset = 1;
    if ((c & 0x80) == 0)
        return c;
    return getCharExtended(c, offset);
}

uint32_t SourceFile::getCharExtended(char c, unsigned& offset)
{
    uint32_t wc;
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

    return '?';
}

Utf8 SourceFile::getLine(long lineNo)
{
    scoped_lock lk(mutex);
    if (isExternal)
    {
        if (allLines.empty())
        {
            const char* pz = (const char*) buffer;
            string      line;
            while (*pz)
            {
                while (*pz && *pz != '\n')
                    line += *pz++;
                allLines.push_back(line);
                line.clear();
                if (*pz)
                    pz++;
            }
        }
    }
    else
    {
        close();

        // Put all lines in a cache, the first time
        // This is slow, but this is ok, as getLine is not called in normal situations
        if (allLines.empty())
        {
            ifstream fle(path, ios::binary);
            if (!fle.is_open())
                return "?";

            string line;
            line.reserve(1024);
            while (std::getline(fle, line))
            {
                allLines.push_back(line);
            }
        }
    }

    if (lineNo >= allLines.size())
        return "";
    return allLines[lineNo];
}

bool SourceFile::report(const Diagnostic& diag, const vector<const Diagnostic*>& notes)
{
    if (silent > 0 && !diag.exceptionError)
        return false;

    scoped_lock lock(g_Log.mutexAccess);

    // Warning to error option ?
    auto errorLevel = diag.errorLevel;
    if (g_CommandLine.warningsAsErrors)
        errorLevel = DiagnosticLevel::Error;

    // Are we in the #semerror block.
    // If so, we do not count the error, as we want to continue
    bool inSemError = false;
    bool isSemError = false;
    if (diag.sourceNode)
    {
        // If we have raised an error for AstNodeKind::CompilerSemError, then this is a real error
        if (diag.sourceNode->kind == AstNodeKind::CompilerSemError)
            isSemError = true;
        else
        {
            auto parent = diag.sourceNode->parent;
            while (parent && parent->kind != AstNodeKind::CompilerSemError)
                parent = parent->parent;
            if (parent)
                inSemError = true;
        }
    }

    if (diag.exceptionError)
    {
        errorLevel = DiagnosticLevel::Error;
        inSemError = false;
        module->criticalErrors++;
    }

    if (errorLevel == DiagnosticLevel::Error)
    {
        if (!inSemError)
        {
            numErrors++;
            module->numErrors++;
        }

        // Do not raise an error if we are waiting for one, during tests
        if ((numTestErrors || inSemError || multipleTestErrors) && !diag.exceptionError && !isSemError)
        {
            if (multipleTestErrors)
                numTestErrors = 0;
            else if (!inSemError)
                numTestErrors--;
            if (g_CommandLine.verboseTestErrors)
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
    }

    if (errorLevel == DiagnosticLevel::Warning)
    {
        if (!inSemError)
        {
            numWarnings++;
            module->numWarnings++;
        }

        // Do not raise a warning if we are waiting for one, during tests
        if (numTestWarnings || multipleTestWarnings)
        {
            if (multipleTestWarnings)
                numTestWarnings = 0;
            else if (!inSemError)
                numTestWarnings--;
            if (g_CommandLine.verboseTestErrors)
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
    }

    // Print error/warning
    diag.report();

    if (errorLevel == DiagnosticLevel::Error)
    {
        // Raise error
        g_Workspace.numErrors++;

        // Callstack
        if (g_byteCodeStack.currentContext)
            g_byteCodeStack.currentContext->canCatchError = true;
        SwagContext* context = (SwagContext*) OS::tlsGetValue(g_tlsContextId);
        if (context && (context->flags & (uint64_t) ContextFlags::ByteCode))
            g_byteCodeStack.log();

        // Error stack trace
        for (int i = context->traceIndex - 1; i >= 0; i--)
        {
            auto sourceFile = g_Workspace.findFile((const char*) context->trace[i]->fileName.buffer);
            if (sourceFile)
            {
                SourceLocation startLoc;
                startLoc.line   = context->trace[i]->lineStart;
                startLoc.column = context->trace[i]->colStart;
                SourceLocation endLoc;
                endLoc.line   = context->trace[i]->lineEnd;
                endLoc.column = context->trace[i]->colEnd;
                Diagnostic diag1({sourceFile, startLoc, endLoc, "", DiagnosticLevel::TraceError});
                diag1.report();
            }
        }

        g_Log.eol();
    }

    if (g_CommandLine.errorNoteOut)
    {
        for (auto note : notes)
            note->report();
    }

    if (errorLevel == DiagnosticLevel::Error)
    {
        SwagContext* context = (SwagContext*) OS::tlsGetValue(g_tlsContextId);
        if (context && (context->flags & (uint64_t) ContextFlags::DevMode))
        {
            OS::errorBox("[Developer Mode]", "Error raised !");
            return false;
        }
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

void SourceFile::computePrivateScopeName()
{
    unique_lock lk(mutex);
    if (!scopeName.empty())
        return;
    scopeName = "__" + name;
    SWAG_ASSERT(scopeName.buffer[scopeName.length() - 4] == '.'); // ".swg"
    scopeName.buffer[scopeName.length() - 4] = 0;
    scopeName.count -= 4;
    Ast::normalizeIdentifierName(scopeName);
}
