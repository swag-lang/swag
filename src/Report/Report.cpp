#include "pch.h"
#include "Report/Report.h"
#include "Backend/ByteCode/Run/ByteCodeStack.h"
#include "Backend/Context.h"
#include "Jobs/SaveGenJob.h"
#include "Report/Diagnostic.h"
#include "Report/Log.h"
#include "Semantic/Scope.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/Ast.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

thread_local int  g_SilentError = 0;
thread_local Utf8 g_SilentErrorMsg;

namespace
{
    void computeAutoRemarks(const Vector<Diagnostic*>& notes)
    {
        Set<void*> doneGenParamsRemarks;
        for (const auto note : notes)
        {
            if (!note->display)
                continue;

            // It can happen that the location has nothing to do with the node where the error occurs
            // (because of #location in a @compilererror for example).
            // So hack to avoid displaying generic information not relevant.
            const auto genCheckNode = note->sourceNode ? note->sourceNode : note->contextNode;
            if (genCheckNode && genCheckNode->token.sourceFile == note->sourceFile)
            {
                // This is a generic instance. Display type replacements.
                if (genCheckNode->ownerFct &&
                    genCheckNode->ownerFct->typeInfo &&
                    !doneGenParamsRemarks.contains(genCheckNode->ownerFct->typeInfo))
                {
                    doneGenParamsRemarks.insert(genCheckNode->ownerFct->typeInfo);
                    const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(genCheckNode->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
                    auto       remarks  = Generic::computeGenericParametersReplacement(typeFunc->replaceTypes, typeFunc->replaceValues);
                    if (!remarks.empty())
                        note->autoRemarks.insert(note->autoRemarks.end(), remarks.begin(), remarks.end());
                }

                if (genCheckNode->ownerStructScope &&
                    genCheckNode->ownerStructScope->owner->typeInfo &&
                    genCheckNode->ownerStructScope->owner->typeInfo->kind == TypeInfoKind::Struct &&
                    !doneGenParamsRemarks.contains(genCheckNode->ownerStructScope->owner->typeInfo))
                {
                    doneGenParamsRemarks.insert(genCheckNode->ownerStructScope->owner->typeInfo);
                    const auto typeStruct = castTypeInfo<TypeInfoStruct>(genCheckNode->ownerStructScope->owner->typeInfo, TypeInfoKind::Struct);
                    auto       remarks    = Generic::computeGenericParametersReplacement(typeStruct->replaceTypes, typeStruct->replaceValues);
                    if (!remarks.empty())
                        note->autoRemarks.insert(note->autoRemarks.end(), remarks.begin(), remarks.end());
                }
            }
        }
    }

    void cleanNotes(Vector<Diagnostic*>& notes)
    {
        // Remove notes without messages
        for (uint32_t i = 0; i < notes.size(); i++)
        {
            if (notes[i]->textMsg.empty())
            {
                notes.erase(notes.begin() + i);
                i--;
                continue;
            }
        }

        // Error message can have different parts.
        // We generate hint and notes...
        const auto   err = notes[0];
        Vector<Utf8> parts;
        Diagnostic::tokenizeError(err->textMsg, parts);
        if (parts.size() > 1)
        {
            err->textMsg = parts[0];

            if (!err->hint.empty())
            {
                const auto newNote = Diagnostic::note(err->sourceFile, err->startLocation, err->endLocation, err->hint);
                if (notes.size() == 1)
                    notes.push_back(newNote);
                else
                    notes.insert(++notes.begin(), newNote);
            }

            err->hint = parts[1];

            for (uint32_t i = 2; i < parts.size(); i++)
            {
                auto newNote = Diagnostic::note(parts[i]);
                notes.push_back(newNote);
            }
        }

        // If multiple notes have the same source code location, then merge them on multiple lines
        for (uint32_t i = 0; i < notes.size(); i++)
        {
            for (uint32_t j = i + 1; j < notes.size(); j++)
            {
                if (notes[j]->startLocation.line == notes[i]->startLocation.line && notes[j]->startLocation.column == notes[i]->startLocation.column)
                {
                    if (!notes[i]->hint.empty())
                        notes[i]->hint += "\n=> ";
                    notes[i]->hint += notes[j]->textMsg;
                    notes[j]->textMsg.clear();
                    continue;
                }
            }
        }

        // Remove notes without messages
        for (uint32_t i = 0; i < notes.size(); i++)
        {
            if (notes[i]->textMsg.empty())
            {
                notes.erase(notes.begin() + i);
                i--;
                continue;
            }
        }

        computeAutoRemarks(notes);

        for (const auto note : notes)
        {
            if (!note->display)
                continue;

            // Transform a note in a hint
            if (note->errorLevel == DiagnosticLevel::Note &&
                note->hint.empty() &&
                note->hasLocation)
            {
                note->showErrorLevel = false;
                note->hint           = note->textMsg;
                note->textMsg.clear();
            }

            note->collectRanges();
        }

        // Move ranges from note to note if they share the same line of code, and they do not overlap
        for (uint32_t idxNote = 0; idxNote < notes.size(); idxNote++)
        {
            const auto note = notes[idxNote];
            if (!note->display || !note->canBeMerged)
                continue;

            auto sourceFile0 = note->sourceFile;
            if (sourceFile0 && sourceFile0->fileForSourceLocation)
                sourceFile0 = sourceFile0->fileForSourceLocation;

            for (const auto note1 : notes)
            {
                if (note == note1)
                    continue;
                if (!note1->display)
                    continue;
                if (note->fromContext != note1->fromContext)
                    continue;

                auto sourceFile1 = note1->sourceFile;
                if (sourceFile1 && sourceFile1->fileForSourceLocation)
                    sourceFile1 = sourceFile1->fileForSourceLocation;

                if (sourceFile0 == sourceFile1 &&
                    note->startLocation.line == note1->startLocation.line &&
                    note->endLocation.line == note1->endLocation.line &&
                    note1->canBeMerged &&
                    !note1->ranges.empty() &&
                    note1->errorLevel == DiagnosticLevel::Note)
                {
                    for (uint32_t i = 0; i < note1->ranges.size(); i++)
                    {
                        bool  canAdd = true;
                        auto& r1     = note1->ranges[i];
                        for (uint32_t j = 0; j < note->ranges.size(); j++)
                        {
                            auto& r0 = note->ranges[j];
                            if (r0.endLocation.column <= r1.startLocation.column)
                                continue;
                            if (r0.startLocation.column >= r1.endLocation.column)
                                continue;

                            // Exact same range, but there's no hint. Eat the hint.
                            if (r0.startLocation == r1.startLocation && r0.endLocation == r1.endLocation)
                            {
                                if (r0.hint.empty())
                                {
                                    r0.hint = r1.hint;
                                    r1.hint.clear();
                                    if (note1->ranges.size() == 1)
                                        note1->display = false;
                                }
                                else if (note1->ranges.size() == 1 && note->ranges.size() == 1)
                                {
                                    r1.mergeNext = true;
                                    note->ranges.insert(note->ranges.begin() + j, r1);
                                    note->remarks.insert(note->remarks.end(), note1->remarks.begin(), note1->remarks.end());
                                    note1->display = false;
                                }
                            }

                            canAdd = false;
                            break;
                        }

                        if (canAdd)
                        {
                            note->ranges.push_back(r1);
                            note->sortRanges();
                            note1->ranges.erase(note1->ranges.begin() + i);
                            i--;
                        }
                    }

                    if (note1->ranges.empty() && note1->textMsg.empty())
                    {
                        note->remarks.insert(note->remarks.end(), note1->remarks.begin(), note1->remarks.end());
                        note1->display = false;
                    }
                }
            }
        }

        // No need to repeat the same source file line reference
        for (uint32_t idxNote = 0; idxNote < notes.size(); idxNote++)
        {
            const auto note = notes[idxNote];
            if (!note->display)
                continue;
            if (!note->showFileName)
                continue;

            auto sourceFile0 = note->sourceFile;
            if (sourceFile0 && sourceFile0->fileForSourceLocation)
                sourceFile0 = sourceFile0->fileForSourceLocation;

            for (uint32_t idxNote1 = idxNote + 1; idxNote1 < notes.size(); idxNote1++)
            {
                const auto note1 = notes[idxNote1];
                if (!note1->display || !note1->hasLocation)
                    continue;
                if (note->fromContext != note1->fromContext)
                    continue;

                auto sourceFile1 = note1->sourceFile;
                if (sourceFile1 && sourceFile1->fileForSourceLocation)
                    sourceFile1 = sourceFile1->fileForSourceLocation;

                if (sourceFile0 && sourceFile1 && sourceFile0->path != sourceFile1->path)
                    break;
                note1->showFileName = false;
            }
        }

        // Compute the space needed to display all line numbers
        uint32_t lineCodeMaxDigits = 0;
        uint32_t minBlanks         = UINT32_MAX;

        for (const auto note : notes)
        {
            lineCodeMaxDigits = std::max(lineCodeMaxDigits, note->lineCodeNumDigits);
            minBlanks         = std::min(minBlanks, note->minBlanks);
        }

        for (const auto note : notes)
        {
            note->lineCodeMaxDigits = lineCodeMaxDigits;
            note->minBlanks         = minBlanks;
        }
    }

#pragma optimize("", off)
    void reportInternal(Log* log, const Diagnostic& err, const Vector<const Diagnostic*>& inNotes)
    {
        if (g_CommandLine.errorOneLine)
        {
            const auto c = new Diagnostic{err};
            c->reportCompact(log);
            log->setDefaultColor();
            return;
        }

        // Make a copy
        Vector<Diagnostic*> notes;
        const auto          c = new Diagnostic{err};
        notes.push_back(c);
        for (const auto n : inNotes)
        {
            if (!n)
                continue;
            notes.push_back(new Diagnostic{*n});
        }

        std::ranges::sort(notes, [](auto& r1, auto& r2) { return r1->fromContext < r2->fromContext; });
        cleanNotes(notes);

        log->writeEol();

        bool              marginBefore = true;
        const Diagnostic* prevN        = nullptr;
        for (const auto n : notes)
        {
            if (!n->display)
                continue;

            n->setupColors();

            if (!n->showErrorLevel)
                marginBefore = true;
            if (n->errorLevel == DiagnosticLevel::Note && marginBefore)
            {
                n->printMarginLineNo(log, 0);

                if (prevN && prevN->fromContext)
                {
                    log->setColor(n->marginBorderColorContext);
                    log->print(LogSymbol::VerticalLineDot);
                }
                else if (!prevN || prevN->fromContext == n->fromContext)
                {
                    log->setColor(n->marginBorderColor);
                    log->print(LogSymbol::VerticalLine);
                }
                log->writeEol();
            }

            n->report(log);
            marginBefore = n->hasContent;
            prevN        = n;
        }

        log->setDefaultColor();
        log->writeEol();
    }

    bool dealWithWarning(const AstAttrUse* attrUse, const Utf8& warnMsg, Diagnostic& err, bool& retResult)
    {
        const auto attrWarn = attrUse->attributes.getAttribute(g_LangSpec->name_Swag_Warn);
        if (!attrWarn)
            return false;

        const auto what    = attrWarn->getValue(g_LangSpec->name_what);
        const auto level   = attrWarn->getValue(g_LangSpec->name_level);
        auto       whatVal = what->text;
        whatVal.trim();

        const auto l = static_cast<WarnLevel>(level->reg.u8);
        if (whatVal.empty())
        {
            if (l == Disable)
                retResult = false;
            else
            {
                retResult = true;
                if (l == Error)
                    err.errorLevel = DiagnosticLevel::Error;
            }

            return true;
        }

        Vector<Utf8> tokens;
        Utf8::tokenize(what->text, '|', tokens);
        for (auto& tk : tokens)
        {
            tk.trim();
            tk.makeLower();
            if (tk == warnMsg)
            {
                if (l == Disable)
                {
                    retResult = false;
                }
                else
                {
                    retResult = true;
                    if (l == Error)
                        err.errorLevel = DiagnosticLevel::Error;
                }

                return true;
            }
        }

        return false;
    }

    bool dealWithWarning(Diagnostic& err)
    {
        if (err.errorLevel != DiagnosticLevel::Warning)
            return true;
        auto node = err.contextNode ? err.contextNode : err.sourceNode;
        if (!node)
            return true;

        // No warning if it's in a dependency
        const auto sourceFile = Report::getDiagFile(err);
        if (sourceFile->imported && sourceFile->imported->is(ModuleKind::Dependency))
            return false;
        const auto module = sourceFile->module;
        if (module->is(ModuleKind::Dependency))
            return false;

        // Get warning identifier
        auto warnMsg = Diagnostic::getErrorId(err.textMsg);
        warnMsg.makeLower();

        // Check attributes in the AST hierarchy
        while (node)
        {
            if (node->is(AstNodeKind::AttrUse))
            {
                const auto attrUse   = castAst<AstAttrUse>(node, AstNodeKind::AttrUse);
                bool       retResult = true;
                if (dealWithWarning(attrUse, warnMsg, err, retResult))
                    return retResult;
            }

            node = node->parent;
        }

        // Check attributes in the file
        auto attrUse = sourceFile->astAttrUse;
        while (attrUse)
        {
            bool retResult = true;
            if (dealWithWarning(attrUse, warnMsg, err, retResult))
                return retResult;
            if (attrUse->hasExtOwner())
                attrUse = attrUse->extOwner()->ownerAttrUse;
        }

        // Check build configuration
        if (module->buildCfg.warnAsErrors.buffer)
        {
            Utf8 txt{static_cast<const char*>(module->buildCfg.warnAsErrors.buffer), static_cast<uint32_t>(module->buildCfg.warnAsErrors.count)};
            txt.trim();
            if (!txt.empty())
            {
                Vector<Utf8> tokens;
                Utf8::tokenize(txt, '|', tokens);
                for (auto& tk : tokens)
                {
                    tk.trim();
                    tk.makeLower();
                    if (tk == warnMsg)
                    {
                        err.errorLevel = DiagnosticLevel::Error;
                        return true;
                    }
                }
            }
        }

        if (module->buildCfg.warnAsWarnings.buffer)
        {
            Utf8 txt{static_cast<const char*>(module->buildCfg.warnAsWarnings.buffer), static_cast<uint32_t>(module->buildCfg.warnAsWarnings.count)};
            txt.trim();
            if (!txt.empty())
            {
                Vector<Utf8> tokens;
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
            Utf8 txt{static_cast<const char*>(module->buildCfg.warnAsDisabled.buffer), static_cast<uint32_t>(module->buildCfg.warnAsDisabled.count)};
            txt.trim();
            if (!txt.empty())
            {
                Vector<Utf8> tokens;
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
            err.errorLevel = DiagnosticLevel::Error;
            return true;
        }

        return true;
    }

    void verboseErrorWarning(const Diagnostic& err, const Vector<const Diagnostic*>& notes)
    {
        bool display = g_CommandLine.verboseErrorsFilter.empty();
        display      = display || err.containsText(g_CommandLine.verboseErrorsFilter);
        for (const auto& n : notes)
            display = display || n->containsText(g_CommandLine.verboseErrorsFilter);
        if (display)
        {
            reportInternal(&g_Log, err, notes);
        }
    }

    bool reportInternal(const Diagnostic& inDiag, const Vector<const Diagnostic*>& inNotes, ByteCodeRunContext* runContext)
    {
        if (g_SilentError > 0 && inDiag.errorLevel != DiagnosticLevel::Exception)
        {
            g_SilentErrorMsg = inDiag.textMsg;
            return false;
        }

        ScopedLock lock(g_Log.mutexAccess);

        const auto copyDiag  = new Diagnostic{inDiag};
        auto       copyNotes = inNotes;
        for (auto note : inDiag.notes)
            copyNotes.push_back(note);

        if (!dealWithWarning(*copyDiag))
            return true;

        auto&       err   = *copyDiag;
        const auto& notes = copyNotes;

        const auto sourceFile = Report::getDiagFile(err);
        SaveGenJob::flush(sourceFile->module);

        switch (err.errorLevel)
        {
            case DiagnosticLevel::Exception:
                ++sourceFile->module->criticalErrors;
                err.errorLevel = DiagnosticLevel::Panic;
                break;

            case DiagnosticLevel::Error:
            case DiagnosticLevel::Panic:
                sourceFile->numErrors++;
                ++sourceFile->module->numErrors;

                // Do not raise an error if we are waiting for one, during tests
                if (sourceFile->hasFlag(FILE_SHOULD_HAVE_ERROR))
                {
                    bool dismiss = true;
                    if (!sourceFile->shouldHaveErrorString.empty())
                    {
                        dismiss   = false;
                        auto str1 = err.textMsg;
                        str1.makeLower();
                        for (const auto& filter : sourceFile->shouldHaveErrorString)
                        {
                            auto str2 = filter;
                            str2.makeLower();
                            if (str1.find(str2) != -1)
                            {
                                dismiss = true;
                                break;
                            }
                        }
                    }

                    if (dismiss)
                    {
                        if (g_CommandLine.verboseErrors)
                            verboseErrorWarning(err, notes);
                        return false;
                    }
                }

                ++g_Workspace->numErrors;
                break;

            case DiagnosticLevel::Warning:
                sourceFile->numWarnings++;
                ++sourceFile->module->numWarnings;

                // Do not raise a warning if we are waiting for one, during tests
                if (sourceFile->hasFlag(FILE_SHOULD_HAVE_WARNING))
                {
                    bool dismiss = true;
                    if (!sourceFile->shouldHaveWarningString.empty())
                    {
                        dismiss = false;
                        for (const auto& filter : sourceFile->shouldHaveWarningString)
                        {
                            auto str1 = err.textMsg;
                            auto str2 = filter;
                            str1.makeLower();
                            str2.makeLower();
                            if (str1.find(str2) != -1)
                            {
                                dismiss = true;
                                break;
                            }
                        }
                    }

                    if (dismiss)
                    {
                        if (g_CommandLine.verboseErrors)
                            verboseErrorWarning(err, notes);
                        return true;
                    }
                }

                ++g_Workspace->numWarnings;
                break;

            default:
                break;
        }

        // Print error/warning
        reportInternal(&g_Log, err, notes);

        if (runContext)
        {
            if (err.errorLevel == DiagnosticLevel::Error || err.errorLevel == DiagnosticLevel::Panic)
                runContext->debugOnFirstError = true;

            if (g_CommandLine.dbgCallStack)
            {
                const SwagContext* context = static_cast<SwagContext*>(OS::tlsGetValue(g_TlsContextId));

                // Bytecode callstack
                if (context && context->flags & static_cast<uint64_t>(ContextFlags::ByteCode))
                {
                    const auto callStack = g_ByteCodeStackTrace->log(runContext);
                    if (!callStack.empty())
                    {
                        g_Log.print("[bytecode callstack]\n", LogColor::Cyan);
                        g_Log.setDefaultColor();
                        g_Log.print(callStack);
                        g_Log.setDefaultColor();
                        g_Log.writeEol();
                    }
                }

                // Error callstack
                if (context && context->traceIndex)
                {
                    Utf8 str;
                    for (uint32_t i = context->traceIndex - 1; i != UINT32_MAX; i--)
                    {
                        const auto sourceFile1 = g_Workspace->getFileByName(static_cast<const char*>(context->traces[i]->fileName.buffer));
                        if (sourceFile1)
                        {
                            str += Log::colorToVTS(LogColor::DarkYellow);
                            str += "error";
                            str += Log::colorToVTS(LogColor::Gray);
                            str += form(" --> %s:%d:%d", sourceFile1->path.cstr(), context->traces[i]->lineStart + 1, context->traces[i]->colStart + 1);
                            str += "\n";
                        }
                    }

                    if (!str.empty())
                    {
                        g_Log.print("[error callstack]\n", LogColor::Cyan);
                        g_Log.setDefaultColor();
                        g_Log.print(str);
                        g_Log.setDefaultColor();
                        g_Log.writeEol();
                    }
                }

                // Runtime callstack
                if (runContext)
                {
                    const auto nativeStack = OS::captureStack();
                    if (!nativeStack.empty())
                    {
                        g_Log.print("[runtime callstack]\n", LogColor::Cyan);
                        g_Log.setDefaultColor();
                        g_Log.print(nativeStack);
                        g_Log.setDefaultColor();
                        g_Log.writeEol();
                    }
                }
            }
        }

        if (err.errorLevel == DiagnosticLevel::Error || err.errorLevel == DiagnosticLevel::Panic)
        {
            if (!OS::isDebuggerAttached())
            {
                if (g_CommandLine.dbgDevMode && !g_CommandLine.dbgCatch)
                {
                    OS::errorBox("[Developer Mode]", "Error raised !");
                    OS::exit(-1);
                    return false;
                }
            }
        }

        return err.errorLevel != DiagnosticLevel::Error && err.errorLevel != DiagnosticLevel::Panic;
    }
}

SourceFile* Report::getDiagFile(const Diagnostic& err)
{
    if (!err.sourceFile && !err.contextFile)
        return nullptr;
    SourceFile* sourceFile = err.contextFile;
    if (!err.contextFile)
        sourceFile = err.sourceFile;
    if (err.sourceNode && err.sourceNode->token.sourceFile == err.sourceFile && err.sourceNode->hasOwnerInline())
        sourceFile = err.sourceNode->ownerInline()->token.sourceFile;
    if (sourceFile->fileForSourceLocation)
        sourceFile = sourceFile->fileForSourceLocation;
    return sourceFile;
}

bool Report::report(const Diagnostic& err, const Vector<const Diagnostic*>& notes, ByteCodeRunContext* runContext)
{
    const bool result = reportInternal(err, notes, runContext);
    return result;
}

bool Report::report(const Diagnostic& err)
{
    return report(err, {});
}

bool Report::error(Module* module, const Utf8& msg)
{
    g_Log.lock();
    g_Log.setColor(LogColor::Red);
    g_Log.write("error: ");
    g_Log.print(form("module %s: ", module->name.cstr()));
    g_Log.print(Diagnostic::oneLiner(msg));
    g_Log.writeEol();
    g_Log.setDefaultColor();
    ++g_Workspace->numErrors;
    ++module->numErrors;
    g_Log.unlock();
    return false;
}

void Report::error(const Utf8& msg)
{
    g_Log.lock();
    g_Log.setColor(LogColor::Red);
    g_Log.write("error: ");
    g_Log.print(Diagnostic::oneLiner(msg));
    if (msg.back() != '\n')
        g_Log.writeEol();
    g_Log.setDefaultColor();
    g_Log.unlock();
}

void Report::errorOS(const Utf8& msg)
{
    g_Log.lock();
    const auto str = OS::getLastErrorAsString();
    g_Log.setColor(LogColor::Red);
    g_Log.print(Diagnostic::oneLiner(msg));
    if (!str.empty())
    {
        g_Log.write(" => ");
        g_Log.print(str);
    }

    g_Log.writeEol();
    g_Log.setDefaultColor();
    g_Log.unlock();
}

bool Report::internalError(AstNode* node, const char* msg)
{
    const Diagnostic err{node, form("[compiler internal] %s", msg)};
    report(err);
    return false;
}

bool Report::internalError(Module* module, const char* msg)
{
    g_Log.lock();
    g_Log.setColor(LogColor::Red);
    g_Log.write("error: ");
    g_Log.print(form("module %s: [compiler internal] ", module->name.cstr()));
    g_Log.write(msg);
    g_Log.writeEol();
    g_Log.setDefaultColor();
    ++g_Workspace->numErrors;
    ++module->numErrors;
    g_Log.unlock();
    return false;
}
