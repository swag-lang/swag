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

namespace Report
{
    void reportNotes(vector<Diagnostic*>& notes, bool verbose = false)
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

    void cleanNotes(const Diagnostic& diag, vector<Diagnostic*>& notes)
    {
        for (auto n : notes)
        {
            auto note = const_cast<Diagnostic*>(n);

            // No multiline range... a test, to reduce verbosity
            if (note->endLocation.line > note->startLocation.line)
            {
                note->showRange             = false;
                note->showMultipleCodeLines = false;
            }

            // No need to repeat the same source file line reference
            if (note->startLocation.line == diag.startLocation.line && note->endLocation.line == diag.endLocation.line && note->sourceFile == diag.sourceFile)
                note->showFileName = false;
        }

        for (auto n : notes)
        {
            auto note = const_cast<Diagnostic*>(n);
            for (auto n1 : notes)
            {
                if (n == n1)
                    continue;
                auto note1 = const_cast<Diagnostic*>(n1);

                // No need to repeat the same source file line reference
                if (note->startLocation.line == note1->startLocation.line && note->endLocation.line == note1->endLocation.line && note->sourceFile == note1->sourceFile)
                    note1->showFileName = false;
            }
        }
    }

    bool report(const Diagnostic& diag, const vector<const Diagnostic*>& inNotes)
    {
        SWAG_ASSERT(diag.sourceFile || diag.contextFile);
        auto sourceFile = diag.contextFile ? diag.contextFile : diag.sourceFile;

        if (sourceFile->silent > 0 && !diag.exceptionError)
        {
            sourceFile->silentError = diag.textMsg;
            return false;
        }

        ScopedLock lock(g_Log.mutexAccess);

        // Make a copy, because we could have to change some 'Diagnostic' content.
        vector<Diagnostic*> notes;
        for (auto d : inNotes)
        {
            auto n = new Diagnostic{*d};
            notes.push_back(n);
        }

        cleanNotes(diag, notes);

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
            sourceFile->module->criticalErrors++;
        }

        if (errorLevel == DiagnosticLevel::Error)
        {
            if (!inSemError)
            {
                sourceFile->numErrors++;
                sourceFile->module->numErrors++;
            }

            // Do not raise an error if we are waiting for one, during tests
            if ((sourceFile->numTestErrors || inSemError || sourceFile->multipleTestErrors) && !diag.exceptionError && !isSemError)
            {
                if (sourceFile->multipleTestErrors)
                    sourceFile->numTestErrors = 0;
                else if (!inSemError)
                    sourceFile->numTestErrors--;
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
                sourceFile->numWarnings++;
                sourceFile->module->numWarnings++;
            }

            // Do not raise a warning if we are waiting for one, during tests
            if (sourceFile->numTestWarnings || sourceFile->multipleTestWarnings)
            {
                if (sourceFile->multipleTestWarnings)
                    sourceFile->numTestWarnings = 0;
                else if (!inSemError)
                    sourceFile->numTestWarnings--;
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
            if (g_ByteCodeStackTrace->currentContext)
                g_ByteCodeStackTrace->currentContext->canCatchError = true;
            SwagContext* context = (SwagContext*) OS::tlsGetValue(g_TlsContextId);
            if (context && (context->flags & (uint64_t) ContextFlags::ByteCode))
                g_ByteCodeStackTrace->log();

            // Error stack trace
            for (int i = context->traceIndex - 1; i >= 0; i--)
            {
                auto sourceFile1 = g_Workspace->findFile((const char*) context->trace[i]->fileName.buffer);
                if (sourceFile1)
                {
                    SourceLocation startLoc;
                    startLoc.line   = context->trace[i]->lineStart;
                    startLoc.column = context->trace[i]->colStart;
                    SourceLocation endLoc;
                    endLoc.line   = context->trace[i]->lineEnd;
                    endLoc.column = context->trace[i]->colEnd;
                    Diagnostic diag1({sourceFile1, startLoc, endLoc, "", DiagnosticLevel::TraceError});
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

    bool report(const Diagnostic& diag, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr)
    {
        vector<const Diagnostic*> notes;
        if (note)
            notes.push_back(note);
        if (note1)
            notes.push_back(note1);
        return report(diag, notes);
    }

    bool error(Module* module, const Utf8& msg)
    {
        g_Log.lock();
        g_Log.setColor(LogColor::Red);
        g_Log.print("error: ");
        g_Log.print(Fmt("module %s: ", module->name.c_str()));
        g_Log.print(msg);
        g_Log.eol();
        g_Log.setDefaultColor();
        g_Log.unlock();

        g_Workspace->numErrors++;
        module->numErrors++;
        return false;
    }

    void error(const Utf8& msg)
    {
        g_Log.lock();
        g_Log.setColor(LogColor::Red);
        g_Log.print(msg);
        if (msg.back() != '\n')
            g_Log.eol();
        g_Log.setDefaultColor();
        g_Log.unlock();
    }

    void errorOS(const Utf8& msg)
    {
        auto str = OS::getLastErrorAsString();
        g_Log.lock();
        g_Log.setColor(LogColor::Red);
        SWAG_ASSERT(msg.back() != '\n');
        g_Log.print(msg);
        if (!str.empty())
        {
            g_Log.print(" : ");
            g_Log.print(str);
        }

        g_Log.eol();
        g_Log.setDefaultColor();
        g_Log.unlock();
    }

    bool internalError(AstNode* node, const char* msg)
    {
        Diagnostic diag{node, Fmt("[compiler internal] %s", msg)};
        report(diag);
        return false;
    }

    bool internalError(Module* module, const char* msg)
    {
        g_Log.lock();
        g_Log.setColor(LogColor::Red);
        g_Log.print("error: ");
        g_Log.print(Fmt("module %s: [compiler internal] ", module->name.c_str()));
        g_Log.print(msg);
        g_Log.eol();
        g_Log.setDefaultColor();
        g_Log.unlock();

        g_Workspace->numErrors++;
        module->numErrors++;
        return false;
    }

}; // namespace Report