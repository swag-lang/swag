#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Global.h"
#include "Diagnostic.h"
#include "TypeManager.h"
#include "ThreadManager.h"
#include "Scope.h"

bool SemanticJob::resolveAttrUse(SemanticContext* context)
{
	context->result = SemanticResult::Done;
    return true;
}
