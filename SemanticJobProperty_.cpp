#include "pch.h"
#include "SemanticJob.h"
#include "SourceFile.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "TypeManager.h"
#include "Workspace.h"

bool SemanticJob::resolveDataOfProperty(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    if (typeInfo->isNative(NativeTypeKind::String))
    {
        auto ptrType         = g_Allocator.alloc<TypeInfoPointer>();
        ptrType->ptrCount    = 1;
        ptrType->finalType   = g_TypeMgr.typeInfoU8;
        ptrType->pointedType = g_TypeMgr.typeInfoU8;
        ptrType->sizeOf      = sizeof(void*);
        ptrType->name        = "*u8";
        ptrType->setConst();
        node->typeInfo    = ptrType;
        node->byteCodeFct = ByteCodeGenJob::emitDataOfProperty;
    }
    else if (typeInfo->kind == TypeInfoKind::Slice)
    {
        auto ptrSlice        = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        auto ptrType         = g_Allocator.alloc<TypeInfoPointer>();
        ptrType->ptrCount    = 1;
        ptrType->finalType   = ptrSlice->pointedType;
        ptrType->pointedType = ptrSlice->pointedType;
        ptrType->sizeOf      = sizeof(void*);
        ptrType->name        = "*" + ptrSlice->pointedType->name;
        if (ptrSlice->isConst())
            ptrType->setConst();
        node->typeInfo    = ptrType;
        node->byteCodeFct = ByteCodeGenJob::emitDataOfProperty;
    }
    else if (typeInfo->kind == TypeInfoKind::Array)
    {
        auto ptrArray        = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto ptrType         = g_Allocator.alloc<TypeInfoPointer>();
        ptrType->ptrCount    = 1;
        ptrType->finalType   = ptrArray->pointedType;
        ptrType->pointedType = ptrArray->pointedType;
        ptrType->sizeOf      = sizeof(void*);
        ptrType->name        = "*" + ptrArray->pointedType->name;
        if (ptrArray->isConst())
            ptrType->setConst();
        node->typeInfo    = ptrType;
        node->byteCodeFct = ByteCodeGenJob::emitDataOfProperty;
    }
    else if (typeInfo->isNative(NativeTypeKind::Any))
    {
        auto ptrType         = g_Allocator.alloc<TypeInfoPointer>();
        ptrType->ptrCount    = 1;
        ptrType->finalType   = g_TypeMgr.typeInfoVoid;
        ptrType->pointedType = g_TypeMgr.typeInfoVoid;
        ptrType->sizeOf      = sizeof(void*);
        ptrType->name        = "*" + g_TypeMgr.typeInfoVoid->name;
        node->typeInfo       = ptrType;
        node->byteCodeFct    = ByteCodeGenJob::emitDataOfProperty;
    }
    else if (typeInfo->kind == TypeInfoKind::Struct)
    {
        if (typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE)
            return context->report({node, "'@dataof' cannot be used on a tuple type"});
        node->typeInfo = typeInfo;
        SWAG_CHECK(resolveUserOp(context, "opData", nullptr, nullptr, node, nullptr, false));
        if (context->result == ContextResult::Pending)
            return true;
        node->typeInfo = g_TypeMgr.typeInfoPVoid;
        if (!node->byteCodeFct)
            node->byteCodeFct = ByteCodeGenJob::emitDataOfProperty;
    }
    else
    {
        return context->report({node, format("'@dataof' cannot be applied to expression of type '%s'", node->typeInfo->name.c_str())});
    }

    return true;
}

bool SemanticJob::resolveCountOfProperty(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    if (typeInfo->isNative(NativeTypeKind::String))
    {
        if (node->flags & AST_VALUE_COMPUTED)
        {
            node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR;
            node->computedValue.reg.u64 = node->computedValue.text.length();
            node->typeInfo              = g_TypeMgr.typeInfoU32;
        }
        else
        {
            node->byteCodeFct = ByteCodeGenJob::emitCountOfProperty;
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Array)
    {
        node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR;
        auto typeArray              = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        node->computedValue.reg.u64 = typeArray->count;
        node->typeInfo              = g_TypeMgr.typeInfoU32;
    }
    else if (typeInfo->kind == TypeInfoKind::Slice)
    {
        node->byteCodeFct = ByteCodeGenJob::emitCountOfProperty;
    }
    else if (typeInfo->kind == TypeInfoKind::TypeList)
    {
        auto typeList = CastTypeInfo<TypeInfoList>(typeInfo, TypeInfoKind::TypeList);
        node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR;
        node->computedValue.reg.u64 = (uint32_t) typeList->childs.size();
        node->typeInfo              = g_TypeMgr.typeInfoU32;
    }
    else if (typeInfo->kind == TypeInfoKind::Variadic || typeInfo->kind == TypeInfoKind::TypedVariadic)
    {
        node->byteCodeFct = ByteCodeGenJob::emitCountOfProperty;
    }
    else if (typeInfo->kind == TypeInfoKind::Struct)
    {
        if (typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE)
            return context->report({node, "'@countof' cannot be used on a tuple type"});
        node->typeInfo = typeInfo;
        SWAG_CHECK(resolveUserOp(context, "opCount", nullptr, nullptr, node, nullptr, false));
        if (context->result == ContextResult::Pending)
            return true;
        node->typeInfo = g_TypeMgr.typeInfoU32;
        if (!node->byteCodeFct)
            node->byteCodeFct = ByteCodeGenJob::emitCountOfProperty;
    }
    else
    {
        typeInfo = TypeManager::concreteType(typeInfo);
        SWAG_VERIFY(typeInfo->flags & TYPEINFO_INTEGER, context->report({node, format("expression should be of type integer, but is '%s'", typeInfo->name.c_str())}));
        SWAG_VERIFY(typeInfo->sizeOf <= 4, context->report({node, format("expression should be a 32 bit integer, but is '%s'", typeInfo->name.c_str())}));
        if (node->flags & AST_VALUE_COMPUTED)
        {
            if (!(typeInfo->flags & TYPEINFO_UNSIGNED))
            {
                switch (typeInfo->nativeType)
                {
                case NativeTypeKind::S8:
                    if (node->computedValue.reg.s8 < 0)
                        return context->report({node, format("constant value should be unsigned, but is '%d'", node->computedValue.reg.s8)});
                    break;
                case NativeTypeKind::S16:
                    if (node->computedValue.reg.s16 < 0)
                        return context->report({node, format("constant value should be unsigned, but is '%d'", node->computedValue.reg.s16)});
                    break;
                case NativeTypeKind::S32:
                    if (node->computedValue.reg.s32 < 0)
                        return context->report({node, format("constant value should be unsigned, but is '%d'", node->computedValue.reg.s32)});
                    break;
                }
            }
        }
        else
        {
            SWAG_VERIFY(typeInfo->flags & TYPEINFO_UNSIGNED, context->report({node, format("expression should be of type unsigned integer, but is '%s'", typeInfo->name.c_str())}));
        }
    }

    node->typeInfo = g_TypeMgr.typeInfoU32;
    return true;
}

bool SemanticJob::resolveTypeOfProperty(SemanticContext* context)
{
    auto  node       = CastAst<AstProperty>(context->node, AstNodeKind::IntrinsicProp);
    auto  sourceFile = context->sourceFile;
    auto& typeTable  = sourceFile->module->typeTable;
    auto  expr       = node->expression;

    SWAG_VERIFY(expr->typeInfo, context->report({expr, "expression cannot be evaluated at compile time"}));
    expr->flags |= AST_NO_BYTECODE;

    // A @typeof as a type in a declaration
    if (node->typeOfAsType)
    {
        node->typeInfo = expr->typeInfo;
    }
    else
    {
        SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, expr->typeInfo, &node->typeInfo, &node->computedValue.reg.u32));
        typeTable.waitForTypeTableJobs(context->job);
        if (context->result != ContextResult::Done)
            return true;
        node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED | AST_VALUE_IS_TYPEINFO;
        SWAG_CHECK(setupIdentifierRef(context, node, node->typeInfo));
    }

    return true;
}

bool SemanticJob::resolveIntrinsicProperty(SemanticContext* context)
{
    auto  node       = CastAst<AstProperty>(context->node, AstNodeKind::IntrinsicProp);
    auto  sourceFile = context->sourceFile;
    auto& typeTable  = sourceFile->module->typeTable;
    auto  expr       = node->expression;

    switch (node->prop)
    {
    case Property::SizeOf:
        SWAG_VERIFY(expr->typeInfo, context->report({expr, "expression cannot be evaluated at compile time"}));
        node->computedValue.reg.u64 = expr->typeInfo->sizeOf;
        node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR;
        node->typeInfo = g_TypeMgr.typeInfoU32;
        break;

    case Property::TypeOf:
        SWAG_CHECK(resolveTypeOfProperty(context));
        return true;

    case Property::KindOf:
        SWAG_VERIFY(expr->typeInfo, context->report({expr, "expression cannot be evaluated at compile time"}));
        SWAG_VERIFY(!(expr->typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE), context->report({expr, "'@kindof' cannot be used on a tuple type"}));
        SWAG_VERIFY(expr->typeInfo->isNative(NativeTypeKind::Any), context->report({expr, format("'@kindof' can only be used with type 'any' ('%s' provided)", expr->typeInfo->name.c_str())}));
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, expr->typeInfo, &node->typeInfo, &node->computedValue.reg.u32));
        typeTable.waitForTypeTableJobs(context->job);
        if (context->result != ContextResult::Done)
            return true;
        node->byteCodeFct = ByteCodeGenJob::emitKindOfProperty;
        node->flags |= AST_R_VALUE;
        SWAG_CHECK(setupIdentifierRef(context, node, node->typeInfo));
        return true;

    case Property::CountOf:
        if (expr->typeInfo->kind == TypeInfoKind::Enum)
        {
            auto typeEnum               = CastTypeInfo<TypeInfoEnum>(expr->typeInfo, TypeInfoKind::Enum);
            node->computedValue.reg.u32 = (uint32_t) typeEnum->values.size();
            node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
            node->typeInfo = g_TypeMgr.typeInfoU32;
            break;
        }

        SWAG_CHECK(checkIsConcrete(context, expr));
        node->inheritComputedValue(expr);
        SWAG_CHECK(resolveCountOfProperty(context, node, expr->typeInfo));
        break;

    case Property::DataOf:
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(resolveDataOfProperty(context, node, expr->typeInfo));
        break;
    }

    return true;
}
