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
#include "LanguageSpec.h"
#include "SaveGenJob.h"

thread_local int    g_SilentError = 0;
thread_local string g_SilentErrorMsg;

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
                    }

                    note->hint += note->textMsg;
                    note->showRange = true;
                }
            }
        }

        for (auto note : notes)
        {
            if (!note->display)
                continue;
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
                    note->display &&
                    !note1->forceSourceFile)
                {
                    note1->showFileName = false;
                }

                // Try to transform a note in a hint
                if (note->hint.empty() &&
                    note->hasRangeLocation &&
                    note1->hint.empty() &&
                    !note1->hasRangeLocation2 &&
                    (!note1->hasRangeLocation ||
                     (note->sourceFile == note1->sourceFile &&
                      note1->startLocation.line == note->startLocation.line &&
                      note1->endLocation.line == note->endLocation.line &&
                      note1->startLocation.column == note->startLocation.column &&
                      note1->endLocation.column == note->endLocation.column)))
                {
                    note->hint = note1->textMsg;
                    note->remarks.insert(note->remarks.end(), note1->remarks.begin(), note1->remarks.end());
                    note1->display = false;
                }

                // Merge hints
                if (note->hint2.empty() &&
                    note1->hint2.empty() &&
                    !note1->hint.empty() &&
                    !note->hasRangeLocation2 &&
                    !note1->hasRangeLocation2 &&
                    note->hasRangeLocation &&
                    note1->hasRangeLocation &&
                    note->sourceFile == note1->sourceFile &&
                    note1->startLocation.line == note->startLocation.line &&
                    note1->endLocation.line == note->endLocation.line &&
                    (note1->startLocation.column > note->endLocation.column ||
                     note1->endLocation.column < note->startLocation.column))
                {
                    note->hasRangeLocation2 = true;
                    note->hint2             = note1->hint;
                    note->startLocation2    = note1->startLocation;
                    note->endLocation2      = note1->endLocation;
                    note->remarks.insert(note->remarks.end(), note1->remarks.begin(), note1->remarks.end());
                    note1->display = false;
                }
            }
        }
    }

    void report(const Diagnostic& diag, const vector<const Diagnostic*>& inNotes, bool verbose)
    {
        if (g_CommandLine.errorCompact)
        {
            auto c = new Diagnostic{diag};
            c->reportCompact(verbose);
            return;
        }

        // Make a copy
        vector<Diagnostic*> notes;
        auto                c = new Diagnostic{diag};
        notes.push_back(c);
        for (auto n : inNotes)
            notes.push_back(new Diagnostic{*n});

        cleanNotes(notes);

        bool prevHasSomething = true;
        for (auto n : notes)
        {
            if (!n->display)
                continue;

            auto hasSomething = n->showFileName || n->showSourceCode || !n->remarks.empty();
            if (!hasSomething && !prevHasSomething)
                n->emptyMarginBefore = false;

            n->report(verbose);

            prevHasSomething = hasSomething;
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

    bool dealWithWarning(AstAttrUse* attrUse, const Utf8& warnMsg, Diagnostic& diag, vector<const Diagnostic*>& inNotes, bool& retResult)
    {
        auto attrWarn = attrUse->attributes.getAttribute(g_LangSpec->name_Swag_Warn);
        if (!attrWarn)
            return false;

        auto what    = attrWarn->getValue(g_LangSpec->name_what);
        auto level   = attrWarn->getValue(g_LangSpec->name_level);
        auto whatVal = what->text;
        whatVal.trim();

        auto l = (WarnLevel) level->reg.u8;
        if (whatVal.empty())
        {
            if (l == WarnLevel::Disable)
                retResult = false;
            else
            {
                retResult = true;
                if (l == WarnLevel::Error)
                    diag.errorLevel = DiagnosticLevel::Error;
            }

            return true;
        }
        else
        {
            vector<Utf8> tokens;
            Utf8::tokenize(what->text, '|', tokens);
            for (auto& tk : tokens)
            {
                tk.trim();
                tk.makeLower();
                if (tk == warnMsg)
                {
                    if (l == WarnLevel::Disable)
                    {
                        retResult = false;
                    }
                    else
                    {
                        retResult = true;
                        if (l == WarnLevel::Error)
                            diag.errorLevel = DiagnosticLevel::Error;
                    }

                    return true;
                }
            }
        }

        return false;
    }

    bool dealWithWarning(Diagnostic& diag, vector<const Diagnostic*>& notes)
    {
        if (diag.errorLevel != DiagnosticLevel::Warning)
            return true;
        if (!diag.raisedOnNode)
            return true;

        // No warning if it's in a dependency
        auto sourceFile = getDiagFile(diag);
        if (sourceFile->imported && sourceFile->imported->kind == ModuleKind::Dependency)
            return false;
        auto module = sourceFile->module;
        if (module->kind == ModuleKind::Dependency)
            return false;

        // Get warning identifier
        Utf8 warnMsg;
        auto pz = diag.textMsg.c_str();
        while (*pz && *pz != '[')
            pz++;
        if (*pz == 0)
            return true;
        pz++;
        while (*pz && *pz != ']')
            warnMsg += *pz++;
        if (*pz == 0)
            return true;
        warnMsg.makeLower();

        // Check attributes in the ast hierarchy
        auto node = diag.raisedOnNode;
        while (node)
        {
            if (node->kind == AstNodeKind::AttrUse)
            {
                auto attrUse   = CastAst<AstAttrUse>(node, AstNodeKind::AttrUse);
                bool retResult = true;
                if (dealWithWarning(attrUse, warnMsg, diag, notes, retResult))
                    return retResult;
            }

            node = node->parent;
        }

        // Check attributes in the file
        auto attrUse = sourceFile->astAttrUse;
        while (attrUse)
        {
            bool retResult = true;
            if (dealWithWarning(attrUse, warnMsg, diag, notes, retResult))
                return retResult;
            if (attrUse->extension && attrUse->extension->owner)
                attrUse = attrUse->extension->owner->ownerAttrUse;
        }

        // Check build configuration
        if (module->buildCfg.warnAsErrors.buffer)
        {
            Utf8 txt{(const char*) module->buildCfg.warnAsErrors.buffer, (uint32_t) module->buildCfg.warnAsErrors.count};
            txt.trim();
            if (!txt.empty())
            {
                vector<Utf8> tokens;
                Utf8::tokenize(txt, '|', tokens);
                for (auto& tk : tokens)
                {
                    tk.trim();
                    tk.makeLower();
                    if (tk == warnMsg)
                    {
                        diag.errorLevel = DiagnosticLevel::Error;
                        return true;
                    }
                }
            }
        }

        if (module->buildCfg.warnAsWarnings.buffer)
        {
            Utf8 txt{(const char*) module->buildCfg.warnAsWarnings.buffer, (uint32_t) module->buildCfg.warnAsWarnings.count};
            txt.trim();
            if (!txt.empty())
            {
                vector<Utf8> tokens;
                Utf8::tokenize(txt, '|', tokens);
                for (auto& tk : tokens)
                {
                    tk.trim();
                    tk.makeLower();
                    if (tk == warnMsg)
                    {
                        return true;
                    }
                }
            }
        }

        if (module->buildCfg.warnAsDisabled.buffer)
        {
            Utf8 txt{(const char*) module->buildCfg.warnAsDisabled.buffer, (uint32_t) module->buildCfg.warnAsDisabled.count};
            txt.trim();
            if (!txt.empty())
            {
                vector<Utf8> tokens;
                Utf8::tokenize(txt, '|', tokens);
                for (auto& tk : tokens)
                {
                    tk.trim();
                    tk.makeLower();
                    if (tk == warnMsg)
                    {
                        return false;
                    }
                }
            }
        }

        if (module->buildCfg.warnDefaultDisabled)
        {
            return false;
        }

        if (module->buildCfg.warnDefaultErrors)
        {
            diag.errorLevel = DiagnosticLevel::Error;
            return true;
        }

        return true;
    }

    bool report(const Diagnostic& inDiag, const vector<const Diagnostic*>& inNotes)
    {
        if (g_SilentError > 0 && !inDiag.exceptionError)
        {
            g_SilentErrorMsg = inDiag.textMsg.c_str();
            return false;
        }

        ScopedLock lock(g_Log.mutexAccess);

        auto copyDiag  = new Diagnostic{inDiag};
        auto copyNotes = inNotes;

        if (!dealWithWarning(*copyDiag, copyNotes))
            return true;

        const auto& diag  = *copyDiag;
        const auto& notes = copyNotes;

        auto sourceFile = getDiagFile(diag);
        SaveGenJob::flush(sourceFile->module);

        auto errorLevel = diag.errorLevel;

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
                    report(diag, notes, true);
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
                    report(diag, notes, true);
                return true;
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
        else if (errorLevel == DiagnosticLevel::Warning)
        {
            g_Workspace->numWarnings++;
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