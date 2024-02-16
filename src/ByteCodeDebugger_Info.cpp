#include "pch.h"
#include "Ast.h"
#include "ByteCode.h"
#include "ByteCodeDebugger.h"
#include "Log.h"
#include "Module.h"
#include "Workspace.h"

BcDbgCommandResult ByteCodeDebugger::cmdInfoFuncs(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
	if (arg.split.size() > 3)
		return BcDbgCommandResult::BadArguments;

	const auto filter = arg.split.size() == 3 ? arg.split[2] : Utf8("");

	Vector<Utf8> all;
	g_Log.setColor(LogColor::Gray);

	uint32_t total = 0;
	for (const auto m : g_Workspace->modules)
	{
		for (const auto bc : m->byteCodeFunc)
		{
			if (!bc->out)
				continue;
			total++;
			if (testNameFilter(bc->getPrintName(), filter) ||
				testNameFilter(bc->getPrintFileName(), filter))
			{
				all.push_back(bc->getPrintRefName());
			}
		}
	}

	if (all.empty())
	{
		printCmdError(FMT("...no match (%d parsed functions)", total));
		return BcDbgCommandResult::Continue;
	}

	ranges::sort(all, [](const Utf8& a, const Utf8& b)
	{
		return strcmp(a.c_str(), b.c_str()) < 0;
	});
	printLong(all);
	return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoModules(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
	if (arg.split.size() > 2)
		return BcDbgCommandResult::BadArguments;

	Vector<Utf8> all;
	for (const auto m : g_Workspace->modules)
		all.push_back(m->name);
	ranges::sort(all, [](const Utf8& a, const Utf8& b)
	{
		return strcmp(a.c_str(), b.c_str()) < 0;
	});
	printLong(all);
	return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoVariables(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
	if (arg.split.size() > 3)
		return BcDbgCommandResult::BadArguments;
	const auto filter = arg.split.size() == 3 ? arg.split[2] : Utf8("");

	Utf8       result;
	const auto m = context->jc.sourceFile->module;

	for (const auto n : m->globalVarsBss)
	{
		if (!n->resolvedSymbolOverload)
			continue;
		uint8_t* addr = m->bssSegment.address(n->resolvedSymbolOverload->computedValue.storageOffset);
		appendTypedValue(context, filter, n, nullptr, addr, result);
	}

	for (const auto n : m->globalVarsMutable)
	{
		if (!n->resolvedSymbolOverload)
			continue;
		uint8_t* addr = m->mutableSegment.address(n->resolvedSymbolOverload->computedValue.storageOffset);
		appendTypedValue(context, filter, n, nullptr, addr, result);
	}

	printLong(result);
	return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoLocals(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
	if (arg.split.size() > 3)
		return BcDbgCommandResult::BadArguments;
	const auto filter = arg.split.size() == 3 ? arg.split[2] : Utf8("");

	if (g_ByteCodeDebugger.cxtBc->localVars.empty())
	{
		printCmdError("no locals");
		return BcDbgCommandResult::Continue;
	}

	Utf8 result;
	for (const auto l : g_ByteCodeDebugger.cxtBc->localVars)
		appendTypedValue(context, filter, l, g_ByteCodeDebugger.cxtBp, nullptr, result);
	printLong(result);

	return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoArgs(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
	if (arg.split.size() > 3)
		return BcDbgCommandResult::BadArguments;
	const auto filter = arg.split.size() == 3 ? arg.split[2] : Utf8("");

	const auto funcDecl = castAst<AstFuncDecl>(g_ByteCodeDebugger.cxtBc->node, AstNodeKind::FuncDecl);
	if (!funcDecl->parameters || funcDecl->parameters->childs.empty())
	{
		printCmdError("no arguments");
		return BcDbgCommandResult::Continue;
	}

	Utf8 result;
	for (const auto l : funcDecl->parameters->childs)
		appendTypedValue(context, filter, l, g_ByteCodeDebugger.cxtBp, nullptr, result);
	printLong(result);

	return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoRegs(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
	if (arg.split.size() > 3)
		return BcDbgCommandResult::BadArguments;

	ValueFormat fmt;
	fmt.isHexa   = true;
	fmt.bitCount = 64;
	if (arg.split.size() > 2)
	{
		if (!getValueFormat(arg.split[2], fmt))
			return BcDbgCommandResult::BadArguments;
	}

	g_Log.setColor(LogColor::Gray);
	for (int i = 0; i < context->getRegCount(g_ByteCodeDebugger.cxtRc); i++)
	{
		auto& regP = context->getRegBuffer(g_ByteCodeDebugger.cxtRc)[i];
		Utf8  str;
		appendLiteralValue(context, str, fmt, &regP);
		str.trim();
		g_Log.print(FMT("%s$r%d%s = ", Log::colorToVTS(LogColor::Name).c_str(), i, Log::colorToVTS(LogColor::Default).c_str()));
		g_Log.print(str);
		g_Log.eol();
	}

	g_Log.print(FMT("%s$sp%s = 0x%016llx\n", Log::colorToVTS(LogColor::Name).c_str(), Log::colorToVTS(LogColor::Default).c_str(), context->sp));
	g_Log.print(FMT("%s$bp%s = 0x%016llx\n", Log::colorToVTS(LogColor::Name).c_str(), Log::colorToVTS(LogColor::Default).c_str(), context->bp));

	return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfo(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
	if (arg.split.size() < 2)
		return BcDbgCommandResult::BadArguments;

	if (arg.split[1] == "locals" || arg.split[1] == "loc")
		return cmdInfoLocals(context, arg);
	if (arg.split[1] == "arguments" || arg.split[1] == "arg")
		return cmdInfoArgs(context, arg);
	if (arg.split[1] == "registers" || arg.split[1] == "reg")
		return cmdInfoRegs(context, arg);
	if (arg.split[1] == "functions" || arg.split[1] == "func")
		return cmdInfoFuncs(context, arg);
	if (arg.split[1] == "variables" || arg.split[1] == "var")
		return cmdInfoVariables(context, arg);
	if (arg.split[1] == "modules")
		return cmdInfoModules(context, arg);

	return BcDbgCommandResult::BadArguments;
}
