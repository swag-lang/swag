#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Global.h"
#include "Scope.h"
#include "TypeManager.h"
#include "SymTable.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "Module.h"

bool SemanticJob::resolveVarDecl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto node       = static_cast<AstVarDecl*>(context->node);

    uint32_t symbolFlags = 0;
    if (node->kind == AstNodeKind::FuncDeclParam)
        symbolFlags |= OVERLOAD_VAR_FUNC_PARAM;
    else if (node->ownerScope->isGlobal())
        symbolFlags |= OVERLOAD_VAR_GLOBAL;
    else
        symbolFlags |= OVERLOAD_VAR_LOCAL;

    // Value
    if (node->astAssignment)
    {
        if ((symbolFlags & OVERLOAD_VAR_GLOBAL) || (symbolFlags & OVERLOAD_VAR_FUNC_PARAM) || (node->astAssignment->flags & AST_CONST_EXPR))
        {
            SWAG_CHECK(executeNode(context, node->astAssignment, true));
            if (context->result == SemanticResult::Pending)
                return true;
        }
    }

    // Find type
    if (node->astType && node->astAssignment)
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, node->astType->typeInfo, node->astAssignment));
        node->typeInfo = node->astType->typeInfo;
    }
    else if (node->astAssignment)
    {
        node->typeInfo = node->astAssignment->typeInfo;
    }
    else if (node->astType)
    {
        node->typeInfo = node->astType->typeInfo;
    }

    node->typeInfo = TypeManager::concreteType(node->typeInfo);
    SWAG_VERIFY(node->typeInfo, sourceFile->report({sourceFile, node->token, format("unable to deduce type of variable '%s'", node->name.c_str())}));

    // Register symbol with its type
    auto overload = node->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile, node, node->typeInfo, SymbolKind::Variable, nullptr, symbolFlags);
    SWAG_CHECK(overload);
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, node->ownerScope, node, SymbolKind::Variable));
    node->resolvedSymbolOverload = overload;

    // Assign value
    auto typeInfo = TypeManager::concreteType(node->typeInfo);
    if (symbolFlags & OVERLOAD_VAR_GLOBAL)
    {
        auto     value    = node->astAssignment ? &node->astAssignment->computedValue : &node->computedValue;
        void*    ptrValue = &value->reg;
        Register storedV[2];
        if (typeInfo->isNative(NativeType::String))
        {
            // Store direct pointer to text. Not sure this is really safe
            storedV[0].pointer = (uint8_t*) value->text.c_str();
            storedV[1].u64     = value->text.length();
            ptrValue           = &storedV;
        }

        overload->storageOffset = sourceFile->module->reserveDataSegment(typeInfo->sizeOf, ptrValue);

        if (typeInfo->isNative(NativeType::String))
        {
            auto strIndex = sourceFile->module->reserveDataSegmentString(value->text);
            sourceFile->module->addDataSegmentInitString(overload->storageOffset, strIndex);
        }
    }
    else if (symbolFlags & OVERLOAD_VAR_LOCAL)
    {
        assert(node->ownerScope);
        assert(node->ownerFct);
        overload->storageOffset = node->ownerScope->startStackSize;
        node->ownerScope->startStackSize += typeInfo->sizeOf;
        node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
        if (node->astAssignment)
            node->byteCodeFct = &ByteCodeGenJob::emitVarDecl;
    }

    // Attributes
    if (context->node->parentAttributes)
    {
        collectAttributes(context, overload->attributes, node->parentAttributes, context->node, AstNodeKind::VarDecl, node->attributeFlags);
    }

    return true;
}
