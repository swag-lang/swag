#include "pch.h"
#include "SemanticJob.h"
#include "Global.h"
#include "TypeManager.h"
#include "Diagnostic.h"
#include "TypeInfo.h"
#include "SourceFile.h"
#include "SymTable.h"
#include "Scope.h"
#include "Ast.h"
#include "AstNode.h"
#include "Module.h"
#include "ThreadManager.h"
#include "ByteCodeGenJob.h"

bool SemanticJob::collectStructLiterals(SemanticContext* context, SourceFile* sourceFile, uint32_t& offset, AstNode* node, SegmentBuffer buffer)
{
    AstStruct* structNode = CastAst<AstStruct>(node, AstNodeKind::StructDecl);
    auto       module     = sourceFile->module;

    uint8_t* ptrStart;
    if (buffer == SegmentBuffer::Constant)
        ptrStart = &module->constantSegment[offset];
    else if (buffer == SegmentBuffer::Data)
        ptrStart = &module->dataSegment[offset];
    else
        ptrStart = nullptr;

    auto ptrDest = ptrStart;
    for (auto child : structNode->content->childs)
    {
        auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);
        if (varDecl->assignment)
        {
            auto  typeInfo = child->typeInfo;
            auto& value    = varDecl->assignment->computedValue;

            if (typeInfo->isNative(NativeType::String))
            {
                Register* storedV  = (Register*) ptrDest;
                storedV[0].pointer = (uint8_t*) value.text.c_str();
                storedV[1].u64     = value.text.length();

                auto stringIndex = module->reserveString(value.text);
                module->addDataSegmentInitString(offset, stringIndex);

                ptrDest += 2 * sizeof(Register);
                offset += 2 * sizeof(Register);
            }
            else if (typeInfo->kind == TypeInfoKind::Native)
            {
                switch (typeInfo->sizeOf)
                {
                case 1:
                    *(uint8_t*) ptrDest = value.reg.u8;
                    ptrDest += 1;
                    offset += 1;
                    break;
                case 2:
                    *(uint16_t*) ptrDest = value.reg.u16;
                    ptrDest += 2;
                    offset += 2;
                    break;
                case 4:
                    *(uint32_t*) ptrDest = value.reg.u32;
                    ptrDest += 4;
                    offset += 4;
                    break;
                case 8:
                    *(uint64_t*) ptrDest = value.reg.u64;
                    ptrDest += 8;
                    offset += 8;
                    break;
                default:
                    return internalError(context, "collectStructLiterals, invalid native type sizeof");
                }
            }
            else
            {
                return internalError(context, "collectStructLiterals, invalid type");
            }
        }
        else if (varDecl->typeInfo->kind == TypeInfoKind::Struct)
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(varDecl->typeInfo, TypeInfoKind::Struct);
            SWAG_CHECK(collectStructLiterals(context, sourceFile, offset, typeStruct->structNode, buffer));
            ptrDest = ptrStart + offset;
        }
    }

    return true;
}

bool SemanticJob::resolveImpl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto node       = CastAst<AstImpl>(context->node, AstNodeKind::Impl);

    // Be sure this is a struct
    auto typeInfo = node->identifier->typeInfo;
    if (typeInfo->kind != TypeInfoKind::Struct)
    {
        Diagnostic diag{sourceFile, node->identifier, format("'%s' is %s and should be a struct", node->identifier->name.c_str(), TypeInfo::getKindName(typeInfo))};
        Diagnostic note{node->identifier->resolvedSymbolOverload->sourceFile, node->identifier->resolvedSymbolOverload->node->token.startLocation, node->identifier->resolvedSymbolOverload->node->token.endLocation, format("this is the definition of '%s'", node->identifier->name.c_str()), DiagnosticLevel::Note};
        return sourceFile->report(diag, &note);
    }

    return true;
}

bool SemanticJob::resolveStruct(SemanticContext* context)
{
    auto node       = CastAst<AstStruct>(context->node, AstNodeKind::StructDecl);
    auto sourceFile = context->sourceFile;
    auto typeInfo   = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);

    typeInfo->structNode = node;
    typeInfo->name       = format("%s", node->name.c_str());

    uint32_t storageOffset = 0;
    uint32_t storageIndex  = 0;
    uint32_t structFlags   = 0;

    for (auto child : node->content->childs)
    {
        if (child->kind != AstNodeKind::VarDecl)
            continue;

        auto varDecl = static_cast<AstVarDecl*>(child);

        // Var is a struct
        if (varDecl->typeInfo->kind == TypeInfoKind::Struct)
        {
            if (varDecl->typeInfo->flags & TYPEINFO_STRUCT_HAS_CONSTRUCTOR)
                structFlags |= TYPEINFO_STRUCT_HAS_CONSTRUCTOR;
        }

        // Var has an initialization
        else if (varDecl->assignment)
        {
            SWAG_VERIFY(varDecl->assignment->flags & AST_CONST_EXPR, sourceFile->report({sourceFile, varDecl->assignment, "cannot evaluate initialization expression at compile time"}));

            auto typeInfoAssignment = varDecl->assignment->typeInfo;
            if (typeInfoAssignment->isNative(NativeType::String))
                structFlags |= TYPEINFO_STRUCT_HAS_CONSTRUCTOR;
            else if (typeInfoAssignment->kind != TypeInfoKind::Native || varDecl->assignment->computedValue.reg.u64)
                structFlags |= TYPEINFO_STRUCT_HAS_CONSTRUCTOR;
        }

        if (!(node->flags & AST_FROM_GENERIC))
        {
            typeInfo->childs.push_back(child->typeInfo);
            typeInfo->sizeOf += child->typeInfo->sizeOf;
            typeInfo->flags |= structFlags;
        }

        child->resolvedSymbolOverload->storageOffset = storageOffset;
        child->resolvedSymbolOverload->storageIndex  = storageIndex;
        storageOffset += child->typeInfo->sizeOf;
        storageIndex++;
    }

    // Add generic parameters
    uint32_t symbolFlags = 0;
    if (!(node->flags & AST_FROM_GENERIC))
    {
        if (node->genericParameters)
        {
            typeInfo->flags |= TYPEINFO_GENERIC;
            symbolFlags |= OVERLOAD_GENERIC;
            for (auto param : node->genericParameters->childs)
            {
                auto funcParam      = g_Pool_typeInfoFuncAttrParam.alloc();
                funcParam->name     = param->name;
                funcParam->typeInfo = param->typeInfo;
                typeInfo->genericParameters.push_back(funcParam);
                typeInfo->sizeOf += param->typeInfo->sizeOf;
            }
        }
    }

	// Compute size of struct, now that we know the real types
    else
    {
        for (auto param : typeInfo->childs)
        {
            typeInfo->sizeOf += param->sizeOf;
        }
    }

    node->typeInfo = g_TypeMgr.registerType(typeInfo);

    // Register symbol with its type
    SWAG_CHECK(node->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile, node, node->typeInfo, SymbolKind::Struct, nullptr, symbolFlags));

    // Byte code generation of code associated with the structure
    node->byteCodeFct               = &ByteCodeGenJob::emitDefaultStruct;
    node->byteCodeJob               = g_Pool_byteCodeGenJob.alloc();
    node->byteCodeJob->sourceFile   = sourceFile;
    node->byteCodeJob->originalNode = node;
    node->byteCodeJob->nodes.push_back(node);
    ByteCodeGenJob::setupBC(context->sourceFile->module, node);
    g_ThreadMgr.addJob(node->byteCodeJob);

    return true;
}
