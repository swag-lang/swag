#include "pch.h"
#include "Diagnostic.h"
#include "Workspace.h"
#include "Module.h"
#include "ByteCodeStack.h"
#include "Context.h"
#include "Ast.h"
#include "LanguageSpec.h"
#include "SaveGenJob.h"
#include "Report.h"

thread_local int  g_SilentError = 0;
thread_local Utf8 g_SilentErrorMsg;

static bool fuzzySameLine(uint32_t line1, uint32_t line2)
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

static void cleanNotes(Vector<Diagnostic*>& notes)
{
    for (auto note : notes)
    {
        if (!note->display)
            continue;

        auto genCheckNode = note->sourceNode ? note->sourceNode : note->raisedOnNode;

        // This is a generic instance. Display type replacements.
        if (genCheckNode &&
            genCheckNode->ownerFct &&
            genCheckNode->ownerFct->typeInfo)
        {
            auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(genCheckNode->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
            auto remarks  = Ast::computeGenericParametersReplacement(typeFunc->replaceTypes);
            if (!remarks.empty())
                note->autoRemarks.insert(note->autoRemarks.end(), remarks.begin(), remarks.end());
        }

        if (genCheckNode &&
            genCheckNode->ownerStructScope &&
            genCheckNode->ownerStructScope->owner->typeInfo &&
            genCheckNode->ownerStructScope->owner->typeInfo->kind == TypeInfoKind::Struct)
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(genCheckNode->ownerStructScope->owner->typeInfo, TypeInfoKind::Struct);
            auto remarks    = Ast::computeGenericParametersReplacement(typeStruct->replaceTypes);
            if (!remarks.empty())
                note->autoRemarks.insert(note->autoRemarks.end(), remarks.begin(), remarks.end());
        }

        if (genCheckNode &&
            genCheckNode->typeInfo &&
            genCheckNode->typeInfo->kind == TypeInfoKind::Struct)
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(genCheckNode->typeInfo, TypeInfoKind::Struct);
            auto remarks    = Ast::computeGenericParametersReplacement(typeStruct->replaceTypes);
            if (!remarks.empty())
                note->autoRemarks.insert(note->autoRemarks.end(), remarks.begin(), remarks.end());
        }

        // Transform a note/help in a hint
        if (note->errorLevel == DiagnosticLevel::Note || note->errorLevel == DiagnosticLevel::Help)
        {
            if (note->hint.empty() && note->hasLocation)
            {
                note->showErrorLevel = false;
                if (!note->noteHeader.empty())
                {
                    note->hint = note->noteHeader;
                    note->hint += " ";
                }

                note->hint += note->textMsg;
                note->textMsg.clear();
                note->showRange = true;
            }
        }

        note->collectRanges();
    }

    for (int inote = 0; inote < notes.size(); inote++)
    {
        auto note = notes[inote];
        if (!note->display)
            continue;
        for (int inote1 = 0; inote1 < notes.size(); inote1++)
        {
            auto note1 = notes[inote1];
            if (note == note1)
                continue;
            if (!note1->display)
                continue;

            auto sourceFile0 = Report::getDiagFile(*note);
            auto sourceFile1 = Report::getDiagFile(*note1);

            // No need to repeat the same source file line reference
            if (fuzzySameLine(note->startLocation.line, note1->startLocation.line) &&
                fuzzySameLine(note->endLocation.line, note1->endLocation.line) &&
                sourceFile0 == sourceFile1 &&
                note->showFileName &&
                note->display &&
                !note1->forceSourceFile &&
                inote1 > inote &&
                note1->textMsg.empty())
            {
                note1->showFileName = false;
            }

            // A note/help without a range can be merged to the hint of the main error message (if not already defined)
            if (note->ranges.size() &&
                note1->ranges.empty() &&
                !note1->forceSourceFile &&
                (note1->errorLevel == DiagnosticLevel::Note || note1->errorLevel == DiagnosticLevel::Help))
            {
                for (auto& r : note->ranges)
                {
                    if (r.errorLevel == DiagnosticLevel::Error || r.errorLevel == DiagnosticLevel::Panic)
                    {
                        if (r.hint.empty())
                        {
                            r.hint = note1->textMsg;
                            note->remarks.insert(note->remarks.end(), note1->remarks.begin(), note1->remarks.end());
                            note1->display = false;
                        }

                        break;
                    }
                }
            }

            // Move ranges from note to note if they share the same line of code, and they do not overlap
            if (note->showRange &&
                note1->showRange &&
                sourceFile0 == sourceFile1 &&
                note->startLocation.line == note1->startLocation.line &&
                note->endLocation.line == note1->endLocation.line &&
                note1->ranges.size() &&
                !note1->forceSourceFile &&
                (note1->errorLevel == DiagnosticLevel::Note || note1->errorLevel == DiagnosticLevel::Help))
            {
                for (size_t i = 0; i < note1->ranges.size(); i++)
                {
                    bool  canAdd = true;
                    auto& r1     = note1->ranges[i];
                    for (size_t j = 0; j < note->ranges.size(); j++)
                    {
                        auto& r0 = note->ranges[j];
                        if (r0.endLocation.column <= r1.startLocation.column)
                            continue;
                        if (r0.startLocation.column >= r1.endLocation.column)
                            continue;

                        // Exact same range, but there's no hint. Eat the hint.
                        if (r0.startLocation == r1.startLocation && r0.endLocation == r1.endLocation && r0.hint.empty())
                        {
                            r0.hint = r1.hint;
                            r1.hint.clear();
                            if (note1->ranges.size() == 1)
                                note1->display = false;
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

    bool        showFileName = false;
    Diagnostic* prevNote     = nullptr;
    for (auto note : notes)
    {
        if (!note->display)
            continue;

        if (note->showFileName || !note->showSourceCode)
        {
            if (prevNote)
                prevNote->closeFileName = true;
            showFileName = true;
        }

        prevNote = note;
    }

    for (size_t i = notes.size() - 1; i >= 0; i--)
    {
        if (notes[i]->display)
        {
            notes[i]->closeFileName = true;
            break;
        }
    }
}

static void reportInternal(const Diagnostic& diag, const Vector<const Diagnostic*>& inNotes, bool verbose)
{
    if (g_CommandLine.errorOneLine)
    {
        auto c = new Diagnostic{diag};
        c->reportCompact(verbose);
        g_Log.setDefaultColor();
        return;
    }

    // Make a copy
    Vector<Diagnostic*> notes;
    auto                c = new Diagnostic{diag};
    notes.push_back(c);
    for (auto n : inNotes)
        notes.push_back(new Diagnostic{*n});

    cleanNotes(notes);
    g_Log.eol();

    bool prevHasSomething = true;
    bool first            = true;

    for (auto n : notes)
    {
        if (!n->display)
            continue;

        auto hasSomething = n->showFileName || n->showSourceCode || !n->remarks.empty();
        if (!hasSomething && !prevHasSomething)
            n->emptyMarginBefore = false;

        n->isNote = !first;
        n->report(verbose);
        first = false;

        prevHasSomething = hasSomething;
    }

    g_Log.setDefaultColor();
    g_Log.eol();
}

static bool dealWithWarning(AstAttrUse* attrUse, const Utf8& warnMsg, Diagnostic& diag, Vector<const Diagnostic*>& inNotes, bool& retResult)
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
        Vector<Utf8> tokens;
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

static bool dealWithWarning(Diagnostic& diag, Vector<const Diagnostic*>& notes)
{
    if (diag.errorLevel != DiagnosticLevel::Warning)
        return true;
    if (!diag.raisedOnNode)
        return true;

    // No warning if it's in a dependency
    auto sourceFile = Report::getDiagFile(diag);
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
        if (attrUse->hasExtOwner())
            attrUse = attrUse->extOwner()->ownerAttrUse;
    }

    // Check build configuration
    if (module->buildCfg.warnAsErrors.buffer)
    {
        Utf8 txt{(const char*) module->buildCfg.warnAsErrors.buffer, (uint32_t) module->buildCfg.warnAsErrors.count};
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
        Utf8 txt{(const char*) module->buildCfg.warnAsDisabled.buffer, (uint32_t) module->buildCfg.warnAsDisabled.count};
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
        diag.errorLevel = DiagnosticLevel::Error;
        return true;
    }

    return true;
}

static bool reportInternal(const Diagnostic& inDiag, const Vector<const Diagnostic*>& inNotes, ByteCodeRunContext* runContext)
{
    if (g_SilentError > 0 && inDiag.errorLevel != DiagnosticLevel::Exception)
    {
        g_SilentErrorMsg = inDiag.textMsg.c_str();
        return false;
    }

    ScopedLock lock(g_Log.mutexAccess);

    auto copyDiag  = new Diagnostic{inDiag};
    auto copyNotes = inNotes;

    if (!dealWithWarning(*copyDiag, copyNotes))
        return true;

    auto& diag  = *copyDiag;
    auto& notes = copyNotes;

    auto sourceFile = Report::getDiagFile(diag);
    SaveGenJob::flush(sourceFile->module);

    switch (diag.errorLevel)
    {
    case DiagnosticLevel::Exception:
        sourceFile->module->criticalErrors++;
        diag.errorLevel = DiagnosticLevel::Panic;
        break;

    case DiagnosticLevel::Error:
    case DiagnosticLevel::Panic:
        sourceFile->numErrors++;
        sourceFile->module->numErrors++;

        // Do not raise an error if we are waiting for one, during tests
        if (sourceFile->shouldHaveError)
        {
            bool dismiss = true;
            if (sourceFile->shouldHaveErrorString.size())
            {
                dismiss = false;
                for (auto& filter : sourceFile->shouldHaveErrorString)
                {
                    auto str1 = diag.textMsg;
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
            else
            {
                /////////////////////////
                /*
                std::ifstream t(sourceFile->path);
                std::stringstream buffer;
                buffer << t.rdbuf();
                Utf8 utf = buffer.str();
                int pos = utf.find("#global testerror");
                if (pos != -1)
                {
                    auto num = diag.textMsg;
                    if (num[0] == '[' && num.length() > 8 && num[8] == ']')
                    {
                        num.remove(0, 1);
                        num.remove(7, num.length() - 7);
                    }
                    auto to = Fmt(" \"%s\"", num.c_str());
                    printf("%s\n", to.c_str());
                    utf.insert(pos + 17, to);

                    std::ofstream out(sourceFile->path);
                    out << utf.c_str();
                    out.close();
                }
                */

                /////////////////////////
            }

            if (dismiss)
            {
                if (g_CommandLine.verboseTestErrors)
                    reportInternal(diag, notes, true);
                return false;
            }
        }

        g_Workspace->numErrors++;
        break;

    case DiagnosticLevel::Warning:
        sourceFile->numWarnings++;
        sourceFile->module->numWarnings++;

        // Do not raise a warning if we are waiting for one, during tests
        if (sourceFile->shouldHaveWarning)
        {
            bool dismiss = true;
            if (sourceFile->shouldHaveWarningString.size())
            {
                dismiss = false;
                for (auto& filter : sourceFile->shouldHaveWarningString)
                {
                    auto str1 = diag.textMsg;
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
                if (g_CommandLine.verboseTestErrors)
                    reportInternal(diag, notes, true);
                return true;
            }
        }

        g_Workspace->numWarnings++;
        break;

    default:
        break;
    }

    // Print error/warning
    reportInternal(diag, notes, false);

#if SWAG_DEV_MODE
    if (diag.errorLevel == DiagnosticLevel::Error || diag.errorLevel == DiagnosticLevel::Panic)
    {
        if (!OS::isDebuggerAttached())
        {
            OS::errorBox("[Developer Mode]", "Error raised !");
            return false;
        }
    }
#endif

    if (runContext)
    {
        if (diag.errorLevel == DiagnosticLevel::Error || diag.errorLevel == DiagnosticLevel::Panic)
            runContext->debugOnFirstError = true;

        if (g_CommandLine.dbgCallStack)
        {
            SwagContext* context = (SwagContext*) OS::tlsGetValue(g_TlsContextId);

            // Bytecode callstack
            if (context && (context->flags & (uint64_t) ContextFlags::ByteCode))
            {
                auto callStack = g_ByteCodeStackTrace->log(runContext);
                if (!callStack.empty())
                {
                    g_Log.print("[bytecode callstack]\n", LogColor::Cyan);
                    g_Log.setDefaultColor();
                    g_Log.print(callStack);
                    g_Log.setDefaultColor();
                    g_Log.eol();
                }
            }

            // Error callstack
            if (context && context->traceIndex)
            {
                Utf8 str;
                for (int i = context->traceIndex - 1; i >= 0; i--)
                {
                    auto sourceFile1 = g_Workspace->findFile((const char*) context->traces[i]->fileName.buffer);
                    if (sourceFile1)
                    {
                        str += Log::colorToVTS(LogColor::DarkYellow);
                        str += "error";
                        str += Log::colorToVTS(LogColor::Gray);
                        str += Fmt(" --> %s:%d:%d", sourceFile1->path.string().c_str(), context->traces[i]->lineStart + 1, context->traces[i]->colStart + 1);
                        str += "\n";
                    }
                }

                if (!str.empty())
                {
                    g_Log.print("[error callstack]\n", LogColor::Cyan);
                    g_Log.setDefaultColor();
                    g_Log.print(str);
                    g_Log.setDefaultColor();
                    g_Log.eol();
                }
            }

            // Runtime callstack
            if (runContext)
            {
                auto nativeStack = OS::captureStack();
                if (!nativeStack.empty())
                {
                    g_Log.print("[runtime callstack]\n", LogColor::Cyan);
                    g_Log.setDefaultColor();
                    g_Log.print(nativeStack);
                    g_Log.setDefaultColor();
                    g_Log.eol();
                }
            }
        }
    }

    return diag.errorLevel == DiagnosticLevel::Error || diag.errorLevel == DiagnosticLevel::Panic ? false : true;
}

SourceFile* Report::getDiagFile(const Diagnostic& diag)
{
    if (!diag.sourceFile && !diag.contextFile)
        return nullptr;
    SourceFile* sourceFile = diag.contextFile;
    if (!diag.contextFile)
        sourceFile = diag.sourceFile;
    if (diag.sourceNode && diag.sourceNode->sourceFile == diag.sourceFile && diag.sourceNode->ownerInline)
        sourceFile = diag.sourceNode->ownerInline->sourceFile;
    if (sourceFile->fileForSourceLocation)
        sourceFile = sourceFile->fileForSourceLocation;
    return sourceFile;
}

bool Report::report(const Diagnostic& diag, const Vector<const Diagnostic*>& notes, ByteCodeRunContext* runContext)
{
    bool result = reportInternal(diag, notes, runContext);
    return result;
}

bool Report::report(const Diagnostic& diag, const Diagnostic* note, const Diagnostic* note1)
{
    Vector<const Diagnostic*> notes;
    if (note)
        notes.push_back(note);
    if (note1)
        notes.push_back(note1);
    return report(diag, notes);
}

bool Report::error(Module* module, const Utf8& msg)
{
    g_Log.lock();
    g_Log.setColor(LogColor::Red);
    g_Log.print("error: ");
    g_Log.print(Fmt("module %s: ", module->name.c_str()));
    g_Log.print(msg);
    g_Log.eol();
    g_Log.setDefaultColor();
    g_Workspace->numErrors++;
    module->numErrors++;
    g_Log.unlock();
    return false;
}

void Report::error(const Utf8& msg)
{
    g_Log.lock();
    g_Log.setColor(LogColor::Red);
    g_Log.print("error: ");
    g_Log.print(msg);
    if (msg.back() != '\n')
        g_Log.eol();
    g_Log.setDefaultColor();
    g_Log.unlock();
}

void Report::errorOS(const Utf8& msg)
{
    g_Log.lock();
    auto str = OS::getLastErrorAsString();
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

bool Report::internalError(AstNode* node, const char* msg)
{
    Diagnostic diag{node, Fmt("[compiler internal] %s", msg)};
    report(diag);
    return false;
}

bool Report::internalError(Module* module, const char* msg)
{
    g_Log.lock();
    g_Log.setColor(LogColor::Red);
    g_Log.print("error: ");
    g_Log.print(Fmt("module %s: [compiler internal] ", module->name.c_str()));
    g_Log.print(msg);
    g_Log.eol();
    g_Log.setDefaultColor();
    g_Workspace->numErrors++;
    module->numErrors++;
    g_Log.unlock();
    return false;
}
