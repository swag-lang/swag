#include "pch.h"
#include "Report.h"
#include "Ast.h"
#include "ByteCodeStack.h"
#include "Context.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"
#include "Log.h"
#include "Module.h"
#include "SaveGenJob.h"
#include "Scope.h"
#include "TypeInfo.h"
#include "Workspace.h"

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
			// (because of @location in a @compilererror for example).
			// So hack to avoid displaying generic information not relevant.
			const auto genCheckNode = note->sourceNode ? note->sourceNode : note->contextNode;
			if (genCheckNode && genCheckNode->sourceFile == note->sourceFile)
			{
				// This is a generic instance. Display type replacements.
				if (genCheckNode->ownerFct &&
					genCheckNode->ownerFct->typeInfo &&
					!doneGenParamsRemarks.contains(genCheckNode->ownerFct->typeInfo))
				{
					doneGenParamsRemarks.insert(genCheckNode->ownerFct->typeInfo);
					const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(genCheckNode->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
					auto       remarks  = Generic::computeGenericParametersReplacement(typeFunc->replaceTypes);
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
					auto       remarks    = Generic::computeGenericParametersReplacement(typeStruct->replaceTypes);
					if (!remarks.empty())
						note->autoRemarks.insert(note->autoRemarks.end(), remarks.begin(), remarks.end());
				}

				/*if (genCheckNode->typeInfo &&
				    genCheckNode->typeInfo->kind == TypeInfoKind::Struct &&
				    !doneGenParamsRemarks.contains(genCheckNode->typeInfo))
				{
				    doneGenParamsRemarks.insert(genCheckNode->typeInfo);
				    const auto typeStruct = castTypeInfo<TypeInfoStruct>(genCheckNode->typeInfo, TypeInfoKind::Struct);
				    auto       remarks    = Generic::computeGenericParametersReplacement(typeStruct->replaceTypes);
				    if (!remarks.empty())
				        note->autoRemarks.insert(note->autoRemarks.end(), remarks.begin(), remarks.end());
				}*/
			}
		}
	}

	void cleanNotes(Vector<Diagnostic*>& notes)
	{
		// Remove notes without messages
		for (int i = 0; i < static_cast<int>(notes.size()); i++)
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

			for (int i = 2; i < static_cast<int>(parts.size()); i++)
			{
				auto newNote = Diagnostic::note(parts[i]);
				notes.push_back(newNote);
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
		for (int inote = 0; inote < static_cast<int>(notes.size()); inote++)
		{
			const auto note = notes[inote];
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

				auto sourceFile1 = note1->sourceFile;
				if (sourceFile1 && sourceFile1->fileForSourceLocation)
					sourceFile1 = sourceFile1->fileForSourceLocation;

				if (sourceFile0 == sourceFile1 &&
					note->startLocation.line == note1->startLocation.line &&
					note->endLocation.line == note1->endLocation.line &&
					note1->canBeMerged &&
					!note1->ranges.empty() &&
					(note1->errorLevel == DiagnosticLevel::Note))
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
		for (int inote = 0; inote < static_cast<int>(notes.size()); inote++)
		{
			const auto note = notes[inote];
			if (!note->display)
				continue;
			if (!note->showFileName)
				continue;

			auto sourceFile0 = note->sourceFile;
			if (sourceFile0 && sourceFile0->fileForSourceLocation)
				sourceFile0 = sourceFile0->fileForSourceLocation;

			for (int inote1 = inote + 1; inote1 < static_cast<int>(notes.size()); inote1++)
			{
				const auto note1 = notes[inote1];
				if (!note1->display || !note1->hasLocation)
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
		int lineCodeMaxDigits = 0;
		for (const auto note : notes)
			lineCodeMaxDigits = max(lineCodeMaxDigits, note->lineCodeNumDigits);
		for (const auto note : notes)
			note->lineCodeMaxDigits = lineCodeMaxDigits;
	}

	void reportInternal(const Diagnostic& diag, const Vector<const Diagnostic*>& inNotes)
	{
		if (g_CommandLine.errorOneLine)
		{
			const auto c = new Diagnostic{diag};
			c->reportCompact();
			g_Log.setDefaultColor();
			return;
		}

		// Make a copy
		Vector<Diagnostic*> notes;
		const auto          c = new Diagnostic{diag};
		notes.push_back(c);
		for (const auto n : inNotes)
			notes.push_back(new Diagnostic{*n});

		cleanNotes(notes);
		g_Log.eol();

		bool marginBefore = true;
		for (const auto n : notes)
		{
			if (!n->display)
				continue;

			n->setupColors();

			if (!n->showErrorLevel)
				marginBefore = true;
			if (n->errorLevel == DiagnosticLevel::Note && marginBefore)
			{
				n->printMarginLineNo(0);
				g_Log.setColor(n->marginBorderColor);
				g_Log.print(LogSymbol::VerticalLineDot);
				g_Log.eol();
			}

			n->report();
			marginBefore = n->hasContent;
		}

		g_Log.setDefaultColor();
		g_Log.eol();
	}

	bool dealWithWarning(AstAttrUse* attrUse, const Utf8& warnMsg, Diagnostic& diag, Vector<const Diagnostic*>& inNotes, bool& retResult)
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
					diag.errorLevel = DiagnosticLevel::Error;
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
						diag.errorLevel = DiagnosticLevel::Error;
				}

				return true;
			}
		}

		return false;
	}

	bool dealWithWarning(Diagnostic& diag, Vector<const Diagnostic*>& notes)
	{
		if (diag.errorLevel != DiagnosticLevel::Warning)
			return true;
		auto node = diag.contextNode ? diag.contextNode : diag.sourceNode;
		if (!node)
			return true;

		// No warning if it's in a dependency
		const auto sourceFile = Report::getDiagFile(diag);
		if (sourceFile->imported && sourceFile->imported->kind == ModuleKind::Dependency)
			return false;
		const auto module = sourceFile->module;
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
		while (node)
		{
			if (node->kind == AstNodeKind::AttrUse)
			{
				const auto attrUse   = castAst<AstAttrUse>(node, AstNodeKind::AttrUse);
				bool       retResult = true;
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
						diag.errorLevel = DiagnosticLevel::Error;
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
			diag.errorLevel = DiagnosticLevel::Error;
			return true;
		}

		return true;
	}

	bool reportInternal(const Diagnostic& inDiag, const Vector<const Diagnostic*>& inNotes, ByteCodeRunContext* runContext)
	{
		if (g_SilentError > 0 && inDiag.errorLevel != DiagnosticLevel::Exception)
		{
			g_SilentErrorMsg = inDiag.textMsg.c_str();
			return false;
		}

		ScopedLock lock(g_Log.mutexAccess);

		const auto copyDiag  = new Diagnostic{inDiag};
		auto       copyNotes = inNotes;
		for (auto note : inDiag.notes)
			copyNotes.push_back(note);

		if (!dealWithWarning(*copyDiag, copyNotes))
			return true;

		auto&       diag  = *copyDiag;
		const auto& notes = copyNotes;

		const auto sourceFile = Report::getDiagFile(diag);
		SaveGenJob::flush(sourceFile->module);

		switch (diag.errorLevel)
		{
		case DiagnosticLevel::Exception:
			++sourceFile->module->criticalErrors;
			diag.errorLevel = DiagnosticLevel::Panic;
			break;

		case DiagnosticLevel::Error:
		case DiagnosticLevel::Panic:
			sourceFile->numErrors++;
			++sourceFile->module->numErrors;

		// Do not raise an error if we are waiting for one, during tests
			if (sourceFile->shouldHaveError)
			{
				bool dismiss = true;
				if (!sourceFile->shouldHaveErrorString.empty())
				{
					dismiss   = false;
					auto str1 = diag.textMsg;
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
					if (g_CommandLine.verboseTestErrors)
					{
						if (g_CommandLine.verboseErrorFilter.empty() || diag.textMsg.containsNoCase(g_CommandLine.verboseErrorFilter))
							reportInternal(diag, notes);
					}

					return false;
				}
			}

			++g_Workspace->numErrors;
			break;

		case DiagnosticLevel::Warning:
			sourceFile->numWarnings++;
			++sourceFile->module->numWarnings;

		// Do not raise a warning if we are waiting for one, during tests
			if (sourceFile->shouldHaveWarning)
			{
				bool dismiss = true;
				if (!sourceFile->shouldHaveWarningString.empty())
				{
					dismiss = false;
					for (const auto& filter : sourceFile->shouldHaveWarningString)
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
					{
						if (g_CommandLine.verboseErrorFilter.empty() || diag.textMsg.containsNoCase(g_CommandLine.verboseErrorFilter))
							reportInternal(diag, notes);
					}

					return true;
				}
			}

			++g_Workspace->numWarnings;
			break;

		default:
			break;
		}

		// Print error/warning
		reportInternal(diag, notes);

		if (runContext)
		{
			if (diag.errorLevel == DiagnosticLevel::Error || diag.errorLevel == DiagnosticLevel::Panic)
				runContext->debugOnFirstError = true;

			if (g_CommandLine.dbgCallStack)
			{
				const SwagContext* context = static_cast<SwagContext*>(OS::tlsGetValue(g_TlsContextId));

				// Bytecode callstack
				if (context && (context->flags & static_cast<uint64_t>(ContextFlags::ByteCode)))
				{
					const auto callStack = g_ByteCodeStackTrace->log(runContext);
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
						const auto sourceFile1 = g_Workspace->findFile(static_cast<const char*>(context->traces[i]->fileName.buffer));
						if (sourceFile1)
						{
							str += Log::colorToVTS(LogColor::DarkYellow);
							str += "error";
							str += Log::colorToVTS(LogColor::Gray);
							str += FMT(" --> %s:%d:%d", sourceFile1->path.string().c_str(), context->traces[i]->lineStart + 1, context->traces[i]->colStart + 1);
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
					const auto nativeStack = OS::captureStack();
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

		if (diag.errorLevel == DiagnosticLevel::Error || diag.errorLevel == DiagnosticLevel::Panic)
		{
			if (!OS::isDebuggerAttached())
			{
				if (g_CommandLine.dbgDevMode)
				{
					OS::errorBox("[Developer Mode]", "Error raised !");
					return false;
				}
			}
		}

		return diag.errorLevel != DiagnosticLevel::Error && diag.errorLevel != DiagnosticLevel::Panic;
	}
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
	const bool result = reportInternal(diag, notes, runContext);
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
	g_Log.print(FMT("module %s: ", module->name.c_str()));
	g_Log.print(Diagnostic::oneLiner(msg));
	g_Log.eol();
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
	g_Log.print("error: ");
	g_Log.print(Diagnostic::oneLiner(msg));
	if (msg.back() != '\n')
		g_Log.eol();
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
		g_Log.print(" => ");
		g_Log.print(str);
	}

	g_Log.eol();
	g_Log.setDefaultColor();
	g_Log.unlock();
}

bool Report::internalError(AstNode* node, const char* msg)
{
	const Diagnostic diag{node, FMT("[compiler internal] %s", msg)};
	report(diag);
	return false;
}

bool Report::internalError(Module* module, const char* msg)
{
	g_Log.lock();
	g_Log.setColor(LogColor::Red);
	g_Log.print("error: ");
	g_Log.print(FMT("module %s: [compiler internal] ", module->name.c_str()));
	g_Log.print(msg);
	g_Log.eol();
	g_Log.setDefaultColor();
	++g_Workspace->numErrors;
	++module->numErrors;
	g_Log.unlock();
	return false;
}
