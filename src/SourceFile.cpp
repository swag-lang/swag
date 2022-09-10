#include "pch.h"
#include "SourceFile.h"
#include "Diagnostic.h"
#include "Workspace.h"
#include "Module.h"
#include "Timer.h"
#include "ByteCodeStack.h"
#include "Context.h"
#include "Ast.h"
#include "ErrorIds.h"
#include "File.h"

const auto BUF_SIZE = 2048;

bool SourceFile::checkFormat()
{
    // Read header
    uint8_t c1 = buffer[0];
    uint8_t c2 = buffer[1];
    uint8_t c3 = buffer[2];
    uint8_t c4 = buffer[3];

    if (c1 == 0xEF && c2 == 0xBB && c3 == 0xBF)
    {
        curBuffer += 3;
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
        report({this, Err(Err0314)});
        return false;
    }

    return true;
}

void SourceFile::setExternalBuffer(char* buf, uint32_t size)
{
    buffer     = buf;
    curBuffer  = buf;
    endBuffer  = buf + size;
    bufferSize = size;
    isExternal = true;
}

bool SourceFile::load()
{
    if (isExternal)
        return true;
    if (buffer)
        return true;

    Timer read(&g_Stats.readFilesTime);

    // Seems that we need 'N' flag to avoid handle to be shared with spawned processes
    FILE* handle = nullptr;
    if (!openFile(&handle, path.c_str(), "rbN"))
    {
        numErrors++;
        module->numErrors++;
        return false;
    }

    // Get file length
    fseek(handle, 0, SEEK_END);
    bufferSize = ftell(handle);
    fseek(handle, 0, SEEK_SET);

    // Read content
    allocBufferSize = (unsigned) Allocator::alignSize(bufferSize + 4);
    buffer          = (char*) g_Allocator.alloc(allocBufferSize);

    if (fread(buffer, 1, bufferSize, handle) != bufferSize)
    {
        numErrors++;
        module->numErrors++;
        g_Allocator.free(buffer, allocBufferSize);
        buffer = nullptr;
        closeFile(&handle);
        g_Log.errorOS(Fmt(Err(Err0153), path.c_str()));
        return false;
    }

    closeFile(&handle);

    buffer[bufferSize]     = 0;
    buffer[bufferSize + 1] = 0;
    buffer[bufferSize + 2] = 0;
    buffer[bufferSize + 3] = 0;
    curBuffer              = buffer;
    endBuffer              = buffer + bufferSize;

    if (!checkFormat())
        return false;

    return true;
}

uint32_t SourceFile::getChar(unsigned& offset)
{
    if (curBuffer >= endBuffer)
    {
        offset = 0;
        return 0;
    }

    uint32_t wc;
    Utf8::decodeUtf8(curBuffer, wc, offset);
    return wc;
}

Utf8 SourceFile::getLine(long lineNo, bool* eof)
{
    ScopedLock lk(mutex);
    auto       checkFile = this;
    if (fileForSourceLocation)
        checkFile = fileForSourceLocation;

    if (isExternal && !fileForSourceLocation)
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
        // Put all lines in a cache, the first time
        // This is slow, but this is ok, as getLine is not called in normal situations
        if (allLines.empty())
        {
            ifstream fle(checkFile->path, ios::binary);
            if (!fle.is_open())
                return "?";

            string line;
            line.reserve(1024);
            while (std::getline(fle, line))
            {
                if (!line.empty() && line.back() == '\r')
                    line.resize(line.size() - 1);
                allLines.push_back(line);
            }
        }
    }

    if (lineNo >= allLines.size())
    {
        if (eof)
            *eof = true;
        return "";
    }
    
    if (eof)
        *eof = false;
    return allLines[lineNo];
}

void SourceFile::computeFileScopeName()
{
    ScopedLock lk(mutex);
    if (!scopeName.empty())
        return;
    scopeName = "__" + name;

    char* pz = strrchr(scopeName.buffer, '.');
    SWAG_ASSERT(pz);
    *pz             = 0;
    scopeName.count = (int) (pz - scopeName.c_str());
    Ast::normalizeIdentifierName(scopeName);
}

void SourceFile::reportNotes(const vector<const Diagnostic*>& notes, bool verbose)
{
    if (g_CommandLine->errorNoteOut)
    {
        bool prevHasSourceCode = true;
        for (auto note : notes)
        {
            // Separator if we have a bunch of notes without source code, and one that comes after, but with source code
            auto hasSourceCode = note->mustPrintCode();
            if (hasSourceCode && !prevHasSourceCode)
            {
                g_Log.print(" |");
                g_Log.eol();
            }
            prevHasSourceCode = hasSourceCode;

            note->report(verbose);
        }
    }
}

bool SourceFile::report(const Diagnostic& diag, const vector<const Diagnostic*>& notes)
{
    if (silent > 0 && !diag.exceptionError)
        return false;

    ScopedLock lock(g_Log.mutexAccess);

    // Warning to error option ?
    auto errorLevel = diag.errorLevel;
    if (g_CommandLine->warningsAsErrors)
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
            if (g_CommandLine->verboseTestErrors)
            {
                diag.report(true);
                reportNotes(notes, true);
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
            if (g_CommandLine->verboseTestErrors)
            {
                diag.report(true);
                reportNotes(notes, true);
            }

            return false;
        }
    }

    // Print error/warning
    diag.report();
    reportNotes(notes);

    if (errorLevel == DiagnosticLevel::Error)
    {
        // Raise error
        g_Workspace->numErrors++;

        // Callstack
        if (g_ByteCodeStackTrace.currentContext)
            g_ByteCodeStackTrace.currentContext->canCatchError = true;
        SwagContext* context = (SwagContext*) OS::tlsGetValue(g_TlsContextId);
        if (context && (context->flags & (uint64_t) ContextFlags::ByteCode))
            g_ByteCodeStackTrace.log();

        // Error stack trace
        for (int i = context->traceIndex - 1; i >= 0; i--)
        {
            auto sourceFile = g_Workspace->findFile((const char*) context->trace[i]->fileName.buffer);
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
    }

#if SWAG_DEV_MODE
    if (errorLevel == DiagnosticLevel::Error)
    {
        if (!OS::isDebuggerAttached())
        {
            OS::errorBox("[Developer Mode]", "Error raised !");
            return false;
        }
    }
#endif

    g_Log.eol();
    return errorLevel == DiagnosticLevel::Error ? false : true;
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

bool SourceFile::internalError(AstNode* node, const char* msg)
{
    report({node, Fmt("[compiler internal] %s", msg)});
    return false;
}

void SourceFile::addGlobalUsing(Scope* scope)
{
    for (auto p : globalUsings)
    {
        if (p->getFullName() == scope->getFullName())
            return;
    }

    globalUsings.push_back(scope);
}