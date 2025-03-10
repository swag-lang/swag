#include "pch.h"
#include "Backend/SCBE/Debug/ScbeDebug.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/SCBE/Debug/ScbeDebugCodeView.h"
#include "Backend/SCBE/Encoder/ScbeCpu.h"
#include "Semantic/Scope.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

ScbeDebugTypeIndex ScbeDebug::getTypeSlice(ScbeCpu& pp, const TypeInfo* typeInfo, TypeInfo* pointedType, ScbeDebugTypeIndex* value)
{
    const auto         tr0 = addTypeRecord(pp);
    ScbeDebugTypeField field;
    tr0->kind           = LF_FIELDLIST;
    field.kind          = LF_MEMBER;
    field.type          = getOrCreatePointerToType(pp, pointedType, false);
    field.value.reg.u32 = 0;
    field.name.setView(g_LangSpec->name_data);
    tr0->lfFieldList.fields.reserve(2);
    tr0->lfFieldList.fields.push_back(field);

    field.kind          = LF_MEMBER;
    field.type          = static_cast<ScbeDebugTypeIndex>(UInt64);
    field.value.reg.u32 = sizeof(void*);
    field.name.setView(g_LangSpec->name_count);
    tr0->lfFieldList.fields.push_back(field);

    const auto tr1               = addTypeRecord(pp);
    tr1->kind                    = LF_STRUCTURE;
    tr1->lfStructure.memberCount = 2;
    tr1->lfStructure.sizeOf      = 2 * sizeof(void*);
    tr1->lfStructure.fieldList   = tr0->index;
    if (typeInfo->isSlice())
    {
        tr1->name = "[..] ";
        tr1->name += pointedType->name; // debugger doesn't like 'const' before a slice name
    }
    else
        tr1->name = typeInfo->name;

    *value = tr1->index;
    return tr1->index;
}

void ScbeDebug::getStructFields(ScbeCpu& pp, ScbeDebugTypeRecord* tr, TypeInfoStruct* typeStruct, uint32_t baseOffset)
{
    tr->lfFieldList.fields.reserve(typeStruct->fields.count);
    for (const auto& p : typeStruct->fields)
    {
        ScbeDebugTypeField field;
        field.kind = LF_MEMBER;
        field.type = getOrCreateType(pp, p->typeInfo);
        field.name.setView(p->name);
        field.value.reg.u32 = baseOffset + p->offset;
        tr->lfFieldList.fields.push_back(field);

        if (p->flags.has(TYPEINFOPARAM_HAS_USING) && p->typeInfo->isStruct())
        {
            const auto typeStructField = castTypeInfo<TypeInfoStruct>(p->typeInfo, TypeInfoKind::Struct);
            getStructFields(pp, tr, typeStructField, baseOffset + p->offset);
        }
    }
}

ScbeDebugTypeIndex ScbeDebug::getOrCreateType(ScbeCpu& pp, TypeInfo* typeInfo, bool forceUnRef)
{
    typeInfo = typeInfo->getConcreteAlias();

    // Simple type
    auto simpleType = getSimpleType(typeInfo);
    if (simpleType != None)
        return simpleType;

    // Pointer
    /////////////////////////////////
    if (typeInfo->isPointer())
    {
        auto typePtr = castTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        return getOrCreatePointerToType(pp, typePtr->pointedType, typePtr->isPointerRef() && !forceUnRef);
    }

    // In the cache
    /////////////////////////////////
    using P                           = Map<TypeInfo*, ScbeDebugTypeIndex>;
    std::pair<P::iterator, bool> iter = pp.dbgMapTypes.insert(P::value_type(typeInfo, 0));
    if (!iter.second)
        return iter.first->second;

    // Slice
    /////////////////////////////////
    if (typeInfo->isSlice())
    {
        auto typeInfoPtr = castTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        return getTypeSlice(pp, typeInfo, typeInfoPtr->pointedType, &iter.first->second);
    }

    // TypedVariadic
    /////////////////////////////////
    if (typeInfo->isVariadic())
    {
        return getTypeSlice(pp, typeInfo, g_TypeMgr->typeInfoAny, &iter.first->second);
    }

    // TypedVariadic
    /////////////////////////////////
    if (typeInfo->isTypedVariadic())
    {
        auto typeInfoPtr = castTypeInfo<TypeInfoVariadic>(typeInfo, TypeInfoKind::TypedVariadic);
        return getTypeSlice(pp, typeInfo, typeInfoPtr->rawType, &iter.first->second);
    }

    // Static array
    /////////////////////////////////
    if (typeInfo->isArray())
    {
        auto typeArr            = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto tr                 = addTypeRecord(pp);
        tr->kind                = LF_ARRAY;
        tr->lfArray.elementType = getOrCreateType(pp, typeArr->pointedType, true);
        tr->lfArray.indexType   = UInt64;
        tr->lfArray.sizeOf      = typeArr->sizeOf;
        iter.first->second      = tr->index;
        return tr->index;
    }

    // Native string
    /////////////////////////////////
    if (typeInfo->isString())
    {
        auto               tr0 = addTypeRecord(pp);
        ScbeDebugTypeField field;
        tr0->kind           = LF_FIELDLIST;
        field.kind          = LF_MEMBER;
        field.type          = UnsignedCharacter | NearPointer64 << 8;
        field.value.reg.u32 = 0;
        field.name.setView(g_LangSpec->name_data);
        tr0->lfFieldList.fields.reserve(2);
        tr0->lfFieldList.fields.push_back(field);

        field.kind          = LF_MEMBER;
        field.type          = static_cast<ScbeDebugTypeIndex>(UInt64);
        field.value.reg.u32 = sizeof(void*);
        field.name.setView(g_LangSpec->name_sizeof);
        tr0->lfFieldList.fields.push_back(field);

        auto tr1                     = addTypeRecord(pp);
        tr1->kind                    = LF_STRUCTURE;
        tr1->lfStructure.memberCount = 2;
        tr1->lfStructure.sizeOf      = 2 * sizeof(void*);
        tr1->lfStructure.fieldList   = tr0->index;
        tr1->name.setView(g_LangSpec->name_string);

        iter.first->second = tr1->index;
        return tr1->index;
    }

    // Interface
    /////////////////////////////////
    if (typeInfo->isInterface())
    {
        auto               tr0 = addTypeRecord(pp);
        ScbeDebugTypeField field;
        tr0->kind           = LF_FIELDLIST;
        field.kind          = LF_MEMBER;
        field.type          = UnsignedCharacter | NearPointer64 << 8;
        field.value.reg.u32 = 0;
        field.name.setView(g_LangSpec->name_data);
        tr0->lfFieldList.fields.reserve(2);
        tr0->lfFieldList.fields.push_back(field);

        field.kind          = LF_MEMBER;
        field.type          = getOrCreatePointerPointerToType(pp, g_Workspace->swagScope.regTypeInfoStruct);
        field.value.reg.u32 = sizeof(void*);
        field.name.setView(g_LangSpec->name_itable);
        tr0->lfFieldList.fields.push_back(field);

        auto tr1                     = addTypeRecord(pp);
        tr1->kind                    = LF_STRUCTURE;
        tr1->lfStructure.memberCount = 2;
        tr1->lfStructure.sizeOf      = 2 * sizeof(void*);
        tr1->lfStructure.fieldList   = tr0->index;
        tr1->name.setView(g_LangSpec->name_interface);

        iter.first->second = tr1->index;
        return tr1->index;
    }

    // Any
    /////////////////////////////////
    if (typeInfo->isAny())
    {
        auto               tr0 = addTypeRecord(pp);
        ScbeDebugTypeField field;
        tr0->kind           = LF_FIELDLIST;
        field.kind          = LF_MEMBER;
        field.type          = UnsignedCharacter | NearPointer64 << 8;
        field.value.reg.u32 = 0;
        field.name.setView(g_LangSpec->name_ptrvalue);
        tr0->lfFieldList.fields.reserve(2);
        tr0->lfFieldList.fields.push_back(field);

        field.kind          = LF_MEMBER;
        field.type          = getOrCreatePointerToType(pp, g_Workspace->swagScope.regTypeInfo, false);
        field.value.reg.u32 = sizeof(void*);
        field.name.setView(g_LangSpec->name_typeinfo);
        tr0->lfFieldList.fields.push_back(field);

        auto tr1                     = addTypeRecord(pp);
        tr1->kind                    = LF_STRUCTURE;
        tr1->lfStructure.memberCount = 2;
        tr1->lfStructure.sizeOf      = 2 * sizeof(void*);
        tr1->lfStructure.fieldList   = tr0->index;
        tr1->name.setView(g_LangSpec->name_any);

        iter.first->second = tr1->index;
        return tr1->index;
    }

    // Structure
    /////////////////////////////////
    if (typeInfo->isStruct())
    {
        TypeInfoStruct* typeStruct = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        auto            sname      = getScopedName(typeStruct->declNode);

        if (typeStruct->isFromGeneric())
        {
            auto pz = strchr(typeStruct->name, '\'');
            if (pz)
                sname += pz;
        }

        // Create a forward reference, in case a field points to the struct itself
        auto tr2                 = addTypeRecord(pp);
        tr2->kind                = LF_STRUCTURE;
        tr2->lfStructure.forward = true;
        tr2->name                = sname;

        iter.first->second = tr2->index;

        // List of fields, after the forward ref
        auto tr0  = addTypeRecord(pp);
        tr0->kind = LF_FIELDLIST;
        getStructFields(pp, tr0, typeStruct, 0);

        tr0->lfFieldList.fields.reserve(typeStruct->methods.count);
        for (auto& p : typeStruct->methods)
        {
            ScbeDebugTypeField field;
            field.kind = LF_ONEMETHOD;
            field.type = getOrCreateType(pp, p->typeInfo);
            field.name = getScopedName(p->typeInfo->declNode);
            tr0->lfFieldList.fields.push_back(field);
        }

        // Struct itself, pointing to the field list
        auto tr1                     = addTypeRecord(pp);
        tr1->kind                    = LF_STRUCTURE;
        tr1->lfStructure.memberCount = static_cast<uint16_t>(typeStruct->fields.size());
        tr1->lfStructure.sizeOf      = static_cast<uint16_t>(typeStruct->sizeOf);
        tr1->lfStructure.fieldList   = tr0->index;
        tr1->name                    = sname;

        iter.first->second = tr1->index;
        return tr1->index;
    }

    // Enum
    /////////////////////////////////
    if (typeInfo->isEnum())
    {
        TypeInfoEnum* typeInfoEnum = castTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        auto          sname        = getScopedName(typeInfoEnum->declNode);

        // List of values
        if (typeInfoEnum->rawType->isNativeInteger())
        {
            auto tr0  = addTypeRecord(pp);
            tr0->kind = LF_FIELDLIST;
            tr0->lfFieldList.fields.reserve(typeInfoEnum->values.count);

            VectorNative<TypeInfoEnum*> collect;
            typeInfoEnum->collectEnums(collect);
            for (auto typeEnum : collect)
            {
                for (auto& value : typeEnum->values)
                {
                    if (!value->value)
                        continue;
                    ScbeDebugTypeField field;
                    field.kind      = LF_ENUMERATE;
                    field.type      = getOrCreateType(pp, value->typeInfo);
                    field.name      = value->name;
                    field.valueType = typeInfoEnum->rawType;
                    field.value     = *value->value;
                    tr0->lfFieldList.fields.push_back(field);
                }
            }

            // Enum itself, pointing to the field list
            auto tr1                   = addTypeRecord(pp);
            tr1->kind                  = LF_ENUM;
            tr1->lfEnum.count          = static_cast<uint16_t>(typeInfoEnum->values.size());
            tr1->lfEnum.fieldList      = tr0->index;
            tr1->lfEnum.underlyingType = getOrCreateType(pp, typeInfoEnum->rawType);
            tr1->name                  = sname;

            iter.first->second = tr1->index;
            return tr1->index;
        }

        return getOrCreateType(pp, typeInfoEnum->rawType);
    }

    // Function
    /////////////////////////////////
    if (typeInfo->isFuncAttr() || typeInfo->isLambdaClosure())
    {
        TypeInfoFuncAttr* typeFunc = castTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
        auto              tr0      = addTypeRecord(pp);

        // Get the arg list type. We construct a string with all parameters to be able to
        // store something in the cache
        Utf8 args;
        for (auto& p : typeFunc->parameters)
        {
            args += p->typeInfo->name;
            args += "@";
        }

        bool isMethod = typeFunc->isMethod();
        auto numArgs  = static_cast<uint16_t>(typeFunc->parameters.size());

        ScbeDebugTypeIndex argsTypeIndex;
        using P1                            = MapUtf8<ScbeDebugTypeIndex>;
        std::pair<P1::iterator, bool> iter1 = pp.dbgMapTypesNames.insert(P1::value_type(args, 0));
        if (iter1.second)
        {
            auto tr1             = addTypeRecord(pp);
            tr1->kind            = LF_ARGLIST;
            tr1->lfArgList.count = numArgs;
            for (auto p : typeFunc->parameters)
            {
                tr1->lfArgList.args.push_back(getOrCreateType(pp, p->typeInfo));
            }

            iter1.first->second = tr1->index;
            argsTypeIndex       = tr1->index;
        }
        else
            argsTypeIndex = iter1.first->second;

        if (isMethod)
        {
            tr0->kind                   = LF_MFUNCTION;
            tr0->lfMFunction.returnType = getOrCreateType(pp, typeFunc->returnType);
            auto typeThis               = castTypeInfo<TypeInfoPointer>(typeFunc->parameters[0]->typeInfo, TypeInfoKind::Pointer);
            tr0->lfMFunction.structType = getOrCreateType(pp, typeThis->pointedType);
            tr0->lfMFunction.thisType   = getOrCreateType(pp, typeThis);
            tr0->lfMFunction.numArgs    = numArgs;
            tr0->lfMFunction.argsType   = argsTypeIndex;
        }
        else
        {
            tr0->kind                   = LF_PROCEDURE;
            tr0->lfProcedure.returnType = getOrCreateType(pp, typeFunc->returnType);
            tr0->lfProcedure.numArgs    = numArgs;
            tr0->lfProcedure.argsType   = argsTypeIndex;
        }

        if (typeInfo->isLambdaClosure())
        {
            auto trp                   = addTypeRecord(pp);
            trp->kind                  = LF_POINTER;
            trp->lfPointer.pointeeType = tr0->index;
            iter.first->second         = trp->index;
            return trp->index;
        }

        iter.first->second = tr0->index;
        return tr0->index;
    }

    switch (typeInfo->sizeOf)
    {
        case 1:
            return UnsignedCharacter;
        case 2:
            return UInt16;
        case 4:
            return UInt32;
        default:
            return UInt64;
    }
}

ScbeDebugTypeIndex ScbeDebug::getSimpleType(const TypeInfo* typeInfo)
{
    if (typeInfo->isNative())
    {
        switch (typeInfo->nativeType)
        {
            case NativeTypeKind::Void:
                return Void;
            case NativeTypeKind::Bool:
                return Boolean8;
            case NativeTypeKind::S8:
                return SByte;
            case NativeTypeKind::S16:
                return Int16;
            case NativeTypeKind::S32:
                return Int32;
            case NativeTypeKind::S64:
                return Int64;
            case NativeTypeKind::U8:
                return Byte;
            case NativeTypeKind::U16:
                return UInt16;
            case NativeTypeKind::U32:
                return UInt32;
            case NativeTypeKind::U64:
                return UInt64;
            case NativeTypeKind::F32:
                return Float32;
            case NativeTypeKind::F64:
                return Float64;
            case NativeTypeKind::Rune:
                return Character32;
            default:
                break;
        }
    }

    return None;
}

ScbeDebugTypeIndex ScbeDebug::getOrCreatePointerToType(ScbeCpu& pp, TypeInfo* typeInfo, bool asRef)
{
    const auto simpleType = getSimpleType(typeInfo);
    if (simpleType != None)
        return simpleType | NearPointer64 << 8;

    // Pointer to something complex
    const auto tr             = addTypeRecord(pp);
    tr->kind                  = LF_POINTER;
    tr->lfPointer.pointeeType = getOrCreateType(pp, typeInfo, !asRef);
    tr->lfPointer.asRef       = asRef;
    return tr->index;
}

ScbeDebugTypeIndex ScbeDebug::getOrCreatePointerPointerToType(ScbeCpu& pp, TypeInfo* typeInfo)
{
    // Pointer to something complex
    const auto tr             = addTypeRecord(pp);
    tr->kind                  = LF_POINTER;
    tr->lfPointer.pointeeType = getOrCreatePointerToType(pp, typeInfo, false);
    return tr->index;
}

ScbeDebugTypeRecord* ScbeDebug::addTypeRecord(ScbeCpu& pp)
{
    const auto tr = pp.dbgTypeRecords.addObj<ScbeDebugTypeRecord>();
    tr->index     = pp.dbgTypeRecordsCount + 0x1000;
    pp.dbgTypeRecordsCount++;
    return tr;
}

Utf8 ScbeDebug::getScopedName(const AstNode* node)
{
    const auto scopedName = node->getScopedName();
    Utf8       result;
    result.reserve(scopedName.allocated);

    auto pz      = scopedName.buffer;
    bool lastDot = false;
    for (uint32_t i = 0; i < scopedName.length(); i++, pz++)
    {
        if (*pz == '.')
        {
            if (lastDot)
                continue;
            lastDot = true;
            result += "::";
            continue;
        }

        result += *pz;
        lastDot = false;
    }

    return result;
}

void ScbeDebug::setLocation(CpuFunction* cpuFct, const ByteCodeInstruction* ip, uint32_t byteOffset)
{
    if (!cpuFct->node || cpuFct->node->isSpecialFunctionGenerated())
        return;

    if (!ip)
    {
        ScbeDebugLine dbgLine;
        dbgLine.line       = cpuFct->node->token.startLocation.line + 1;
        dbgLine.byteOffset = byteOffset;
        ScbeDebugLines dbgLines;
        dbgLines.sourceFile = cpuFct->node->token.sourceFile;
        dbgLines.lines.push_back(dbgLine);
        cpuFct->dbgLines.push_back(dbgLines);
        return;
    }

    if (ip->node->is(AstNodeKind::FuncDecl))
    {
        return;
    }

    const auto loc = ByteCode::getLocation(cpuFct->bc, ip, cpuFct->bc->sourceFile && cpuFct->bc->sourceFile->module->buildCfg.backendDebugInline);
    if (!loc.location)
        return;

    // Update begin of start scope
    auto scope = ip->node->ownerScope;
    while (true)
    {
        if (scope->backendStart == 0)
            scope->backendStart = byteOffset;
        scope->backendEnd = byteOffset;
        if (scope->is(ScopeKind::Function))
            break;
        scope = scope->parentScope;
    }

    SWAG_ASSERT(!cpuFct->dbgLines.empty());

    AstFuncDecl* inlined = nullptr;
    if (ip->node && ip->node->hasOwnerInline())
        inlined = ip->node->ownerInline()->func;

    if (cpuFct->dbgLines.back().sourceFile != loc.file || inlined != cpuFct->dbgLines.back().inlined)
    {
        ScbeDebugLines dbgLines;
        dbgLines.sourceFile = loc.file;
        dbgLines.inlined    = inlined;
        cpuFct->dbgLines.push_back(dbgLines);
    }

    // Add a new line if it is different from the previous one
    auto& lines = cpuFct->dbgLines.back().lines;
    if (lines.empty())
    {
        lines.push_back({.line = loc.location->line + 1, .byteOffset = byteOffset});
    }
    else if (lines.back().line != loc.location->line + 1)
    {
        if (lines.back().byteOffset == byteOffset)
            lines.back().line = loc.location->line + 1;
        else
            lines.push_back({.line = loc.location->line + 1, .byteOffset = byteOffset});
    }
}
