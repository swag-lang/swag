#pragma once
#include "Generic.h"

struct AstNode;
struct JobContext;
struct TypeInfo;
struct ErrorContext;
struct Diagnostic;
struct GenericReplaceType;
struct SourceFile;

enum class ErrCxtStepKind
{
	Note,
	Generic,
	Inline,
	CompileTime,
	ValidIf,
	ValidIfx,
	Export,
	HereIs,
};

struct ErrorCxtStep
{
	AstNode*                            node = nullptr;
	ErrCxtStepKind                      type = ErrCxtStepKind::Note;
	VectorMap<Utf8, GenericReplaceType> replaceTypes;
	bool                                locIsToken = false;
	bool                                hide       = false;
	function<Utf8()>                    err;
};

struct PushErrCxtStep
{
	PushErrCxtStep(ErrorContext* context, AstNode* node, ErrCxtStepKind kind, const function<Utf8()>& err, bool locIsToken = false);
	~PushErrCxtStep();
	ErrorContext* cxt;
};

struct ErrorContext
{
	void extract(Diagnostic& diagnostic, Vector<const Diagnostic*>& notes);
	bool report(const Diagnostic& err, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr);
	bool report(const Diagnostic& err, const Vector<const Diagnostic*>& notes);
	bool checkSizeOverflow(const char* typeOverflow, uint64_t value, uint64_t maxValue);

	void reset()
	{
		errCxtSteps.clear();
		sourceFile  = nullptr;
		node        = nullptr;
		hasError    = false;
		silentError = 0;
	}

	Vector<ErrorCxtStep> errCxtSteps;

	SourceFile* sourceFile = nullptr;
	AstNode*    node       = nullptr;

	uint32_t silentError = false;

	bool hasError = false;
};
