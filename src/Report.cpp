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
    bool fuzzySameLine(uint32_t line1, uint32_t line2)
    {
        if (line1 == line2)
            return true;
        if (line1 == line2 + 1)
            return true;
        if (line1 == line2 + 2)
            return true;
        if (line1 + 1 == line2)
            return true;
        if (line1 + 2 == line2)
            return true;
        return false;
    }

    void cleanNotes(vector<Diagnostic*>& notes)
    {
        for (auto note : notes)
        {
            if (!note->display)
                continue;

            // No multiline range... a test, to reduce verbosity
            if (note->endLocation.line > note->startLocation.line)
            {
                note->showRange             = false;
                note->showMultipleCodeLines = false;
            }

            for (auto note1 : notes)
            {
                if (note == note1)
                    continue;
                if (!note1->display)
                    continue;

                // No need to repeat the same source file line reference
                if (fuzzySameLine(note->startLocation.line, note1->startLocation.line) &&
                    fuzzySameLine(note->endLocation.line, note1->endLocation.line) &&
                    note->sourceFile == note1->sourceFile &&
                    note->showFileName &&
                    !note1->forceSourceFile)
                {
                    note1->showFileName = false;
                }

                // Try to transform a note in a hint
                if (note->hint.empty() &&
                        note1->hint.empty() &&
                        !note1->hasRangeLocation2 &&
                        !note1->hasRangeLocation ||
                    (note1->startLocation.line == note->startLocation.line &&
                     note1->endLocation.line == note->endLocation.line &&
                     note1->startLocation.column == note->startLocation.column &&
                     note1->endLocation.column == note->endLocation.column))
                {
                    note->hint = note1->textMsg;
                    note->remarks.insert(note->remarks.end(), note1->remarks.begin(), note1->remarks.end());
                    note1->display = false;
                }
            }

            // Transform a note/help in a hint
            if (note->errorLevel == DiagnosticLevel::Note || note->errorLevel == DiagnosticLevel::Help)
            {
                if (note->hint.empty() && note->hasRangeLocation2 == false && note->hasRangeLocation)
                {
                    note->showErrorLevel = false;
                    if (!note->noteHeader.empty())
                    {
                        note->hint = note->noteHeader;
                        note->hint += " ";
                        note->hint += note->textMsg;
                    }
                    else
                        note->hint = note->textMsg;
                    note->showRange = true;
                }
            }

        }
    }

    void report(const Diagnostic& diag, const vector<const Diagnostic*>& inNotes, bool verbose)
    {
        if (g_CommandLine.errorCompact)
        {
            diag.report(verbose);
            return;
        }

        // Make a copy
        vector<Diagnostic*> notes;
        notes.push_back(new Diagnostic{diag});
        for (auto n: inNotes)
            notes.push_back(new Diagnostic{*n});

        cleanNotes(notes);
        for (auto n : notes)
        {
            if (n->display)
                n->report(verbose);
        }

        g_Log.eol();
    }

    SourceFile* getDiagFile(const Diagnostic& diag)
    {
        SWAG_ASSERT(diag.sourceFile || diag.contextFile);
        SourceFile* sourceFile = diag.contextFile;
        if (!diag.contextFile)
            sourceFile = diag.sourceFile;
        if (diag.sourceNode && diag.sourceNode->sourceFile == diag.sourceFile && diag.sourceNode->ownerInline)
            sourceFile = diag.sourceNode->ownerInline->sourceFile;
        if (sourceFile->fileForSourceLocation)
            sourceFile = sourceFile->fileForSourceLocation;
        return sourceFile;
    }

    bool report(const Diagnostic& diag, const vector<const Diagnostic*>& notes)
    {
        auto sourceFile = getDiagFile(diag);

        if (sourceFile->silent > 0 && !diag.exceptionError)
        {
            sourceFile->silentError = diag.textMsg;
            return false;
        }

        ScopedLock lock(g_Log.mutexAccess);

        // Warning to error option ?
        auto errorLevel = diag.errorLevel;
        if (g_CommandLine.warningsAsErrors)
            errorLevel = DiagnosticLevel::Error;

        if (diag.exceptionError)
        {
            errorLevel = DiagnosticLevel::Error;
            sourceFile->module->criticalErrors++;
        }

        if (errorLevel == DiagnosticLevel::Error)
        {
            sourceFile->numErrors++;
            sourceFile->module->numErrors++;

            // Do not raise an error if we are waiting for one, during tests
            if ((sourceFile->numTestErrors || sourceFile->multipleTestErrors) && !diag.exceptionError)
            {
                if (sourceFile->multipleTestErrors)
                    sourceFile->numTestErrors = 0;
                else
                    sourceFile->numTestErrors--;
                if (g_CommandLine.verboseTestErrors)
                {
                    report(diag, notes, true);
                }

                return false;
            }
        }

        if (errorLevel == DiagnosticLevel::Warning)
        {
            sourceFile->numWarnings++;
            sourceFile->module->numWarnings++;

            // Do not raise a warning if we are waiting for one, during tests
            if (sourceFile->numTestWarnings || sourceFile->multipleTestWarnings)
            {
                if (sourceFile->multipleTestWarnings)
                    sourceFile->numTestWarnings = 0;
                else
                    sourceFile->numTestWarnings--;
                if (g_CommandLine.verboseTestErrors)
                {
                    report(diag, notes, true);
                }

                return false;
            }
        }

        // Print error/warning
        report(diag, notes, false);

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