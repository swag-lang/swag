#include "pch.h"
#include "BackendSCBE.h"
#include "ByteCode.h"
#include "EncoderCPU.h"
#include "EncoderDebug.h"
#include "EncoderDebug_CodeView.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "TypeManager.h"
#include "Version.h"
#include "Workspace.h"

DbgTypeIndex EncoderDebug::getTypeSlice(EncoderCPU& pp, TypeInfo* typeInfo, TypeInfo* pointedType, DbgTypeIndex* value)
{
    auto         tr0 = addTypeRecord(pp);
    DbgTypeField field;
    tr0->kind           = LF_FIELDLIST;
    field.kind          = LF_MEMBER;
    field.type          = getOrCreatePointerToType(pp, pointedType, false);
    field.value.reg.u32 = 0;
    field.name.setView(g_LangSpec->name_data);
    tr0->LF_FieldList.fields.reserve(2);
    tr0->LF_FieldList.fields.emplace_back(std::move(field));

    field.kind          = LF_MEMBER;
    field.type          = (DbgTypeIndex) (SimpleTypeKind::UInt64);
    field.value.reg.u32 = sizeof(void*);
    field.name.setView(g_LangSpec->name_count);
    tr0->LF_FieldList.fields.emplace_back(std::move(field));

    auto tr1                      = addTypeRecord(pp);
    tr1->kind                     = LF_STRUCTURE;
    tr1->LF_Structure.memberCount = 2;
    tr1->LF_Structure.sizeOf      = 2 * sizeof(void*);
    tr1->LF_Structure.fieldList   = tr0->index;
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

void EncoderDebug::getStructFields(EncoderCPU& pp, DbgTypeRecord* tr, TypeInfoStruct* typeStruct, uint32_t baseOffset)
{
    tr->LF_FieldList.fields.reserve(typeStruct->fields.count);
    for (auto& p : typeStruct->fields)
    {
        DbgTypeField field;
        field.kind = LF_MEMBER;
        field.type = getOrCreateType(pp, p->typeInfo);
        field.name.setView(p->name);
        field.value.reg.u32 = baseOffset + p->offset;
        tr->LF_FieldList.fields.emplace_back(std::move(field));

        if (p->flags & TYPEINFOPARAM_HAS_USING && p->typeInfo->isStruct())
        {
            auto typeStructField = CastTypeInfo<TypeInfoStruct>(p->typeInfo, TypeInfoKind::Struct);
            getStructFields(pp, tr, typeStructField, baseOffset + p->offset);
        }
    }
}

DbgTypeIndex EncoderDebug::getOrCreateType(EncoderCPU& pp, TypeInfo* typeInfo, bool forceUnRef)
{
    typeInfo = typeInfo->getConcreteAlias();

    // Simple type
    auto simpleType = getSimpleType(typeInfo);
    if (simpleType != SimpleTypeKind::None)
        return simpleType;

    // Pointer
    /////////////////////////////////
    if (typeInfo->isPointer())
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        return getOrCreatePointerToType(pp, typePtr->pointedType, typePtr->isPointerRef() && !forceUnRef);
    }

    // In the cache
    /////////////////////////////////
    using P                      = Map<TypeInfo*, DbgTypeIndex>;
    pair<P::iterator, bool> iter = pp.dbgMapTypes.insert(P::value_type(typeInfo, 0));
    if (!iter.second)
        return iter.first->second;

    // Slice
    /////////////////////////////////
    if (typeInfo->isSlice())
    {
        auto typeInfoPtr = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
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
        auto typeInfoPtr = CastTypeInfo<TypeInfoVariadic>(typeInfo, TypeInfoKind::TypedVariadic);
        return getTypeSlice(pp, typeInfo, typeInfoPtr->rawType, &iter.first->second);
    }

    // Static array
    /////////////////////////////////
    if (typeInfo->isArray())
    {
        auto typeArr             = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto tr                  = addTypeRecord(pp);
        tr->kind                 = LF_ARRAY;
        tr->LF_Array.elementType = getOrCreateType(pp, typeArr->pointedType, true);
        tr->LF_Array.indexType   = SimpleTypeKind::UInt64;
        tr->LF_Array.sizeOf      = typeArr->sizeOf;
        iter.first->second       = tr->index;
        return tr->index;
    }

    // Native string
    /////////////////////////////////
    if (typeInfo->isString())
    {
        auto         tr0 = addTypeRecord(pp);
        DbgTypeField field;
        tr0->kind           = LF_FIELDLIST;
        field.kind          = LF_MEMBER;
        field.type          = (DbgTypeIndex) (SimpleTypeKind::UnsignedCharacter | (NearPointer64 << 8));
        field.value.reg.u32 = 0;
        field.name.setView(g_LangSpec->name_data);
        tr0->LF_FieldList.fields.reserve(2);
        tr0->LF_FieldList.fields.emplace_back(std::move(field));

        field.kind          = LF_MEMBER;
        field.type          = (DbgTypeIndex) (SimpleTypeKind::UInt64);
        field.value.reg.u32 = sizeof(void*);
        field.name.setView(g_LangSpec->name_sizeof);
        tr0->LF_FieldList.fields.emplace_back(std::move(field));

        auto tr1                      = addTypeRecord(pp);
        tr1->kind                     = LF_STRUCTURE;
        tr1->LF_Structure.memberCount = 2;
        tr1->LF_Structure.sizeOf      = 2 * sizeof(void*);
        tr1->LF_Structure.fieldList   = tr0->index;
        tr1->name.setView(g_LangSpec->name_string);

        iter.first->second = tr1->index;
        return tr1->index;
    }

    // Interface
    /////////////////////////////////
    if (typeInfo->isInterface())
    {
        auto         tr0 = addTypeRecord(pp);
        DbgTypeField field;
        tr0->kind           = LF_FIELDLIST;
        field.kind          = LF_MEMBER;
        field.type          = (DbgTypeIndex) (SimpleTypeKind::UnsignedCharacter | (NearPointer64 << 8));
        field.value.reg.u32 = 0;
        field.name.setView(g_LangSpec->name_data);
        tr0->LF_FieldList.fields.reserve(2);
        tr0->LF_FieldList.fields.emplace_back(std::move(field));

        field.kind          = LF_MEMBER;
        field.type          = getOrCreatePointerPointerToType(pp, g_Workspace->swagScope.regTypeInfoStruct);
        field.value.reg.u32 = sizeof(void*);
        field.name.setView(g_LangSpec->name_itable);
        tr0->LF_FieldList.fields.emplace_back(std::move(field));

        auto tr1                      = addTypeRecord(pp);
        tr1->kind                     = LF_STRUCTURE;
        tr1->LF_Structure.memberCount = 2;
        tr1->LF_Structure.sizeOf      = 2 * sizeof(void*);
        tr1->LF_Structure.fieldList   = tr0->index;
        tr1->name.setView(g_LangSpec->name_interface);

        iter.first->second = tr1->index;
        return tr1->index;
    }

    // Any
    /////////////////////////////////
    if (typeInfo->isAny())
    {
        auto         tr0 = addTypeRecord(pp);
        DbgTypeField field;
        tr0->kind           = LF_FIELDLIST;
        field.kind          = LF_MEMBER;
        field.type          = (DbgTypeIndex) (SimpleTypeKind::UnsignedCharacter | (NearPointer64 << 8));
        field.value.reg.u32 = 0;
        field.name.setView(g_LangSpec->name_ptrvalue);
        tr0->LF_FieldList.fields.reserve(2);
        tr0->LF_FieldList.fields.emplace_back(std::move(field));

        field.kind          = LF_MEMBER;
        field.type          = getOrCreatePointerToType(pp, g_Workspace->swagScope.regTypeInfo, false);
        field.value.reg.u32 = sizeof(void*);
        field.name.setView(g_LangSpec->name_typeinfo);
        tr0->LF_FieldList.fields.emplace_back(std::move(field));

        auto tr1                      = addTypeRecord(pp);
        tr1->kind                     = LF_STRUCTURE;
        tr1->LF_Structure.memberCount = 2;
        tr1->LF_Structure.sizeOf      = 2 * sizeof(void*);
        tr1->LF_Structure.fieldList   = tr0->index;
        tr1->name.setView(g_LangSpec->name_any);

        iter.first->second = tr1->index;
        return tr1->index;
    }

    // Structure
    /////////////////////////////////
    if (typeInfo->isStruct())
    {
        TypeInfoStruct* typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        auto            sname      = EncoderDebug::getScopedName(typeStruct->declNode);

        if (typeStruct->flags & TYPEINFO_FROM_GENERIC)
        {
            auto pz = strchr(typeStruct->name.c_str(), '\'');
            if (pz)
                sname += pz;
        }

        // Create a forward reference, in case a field points to the struct itself
        auto tr2                  = addTypeRecord(pp);
        tr2->kind                 = LF_STRUCTURE;
        tr2->LF_Structure.forward = true;
        tr2->name                 = sname;

        iter.first->second = tr2->index;

        // List of fields, after the forward ref
        auto tr0  = addTypeRecord(pp);
        tr0->kind = LF_FIELDLIST;
        getStructFields(pp, tr0, typeStruct, 0);

        tr0->LF_FieldList.fields.reserve(typeStruct->methods.count);
        for (auto& p : typeStruct->methods)
        {
            DbgTypeField field;
            field.kind = LF_ONEMETHOD;
            field.type = getOrCreateType(pp, p->typeInfo);
            field.name = EncoderDebug::getScopedName(p->typeInfo->declNode);
            tr0->LF_FieldList.fields.emplace_back(std::move(field));
        }

        // Struct itself, pointing to the field list
        auto tr1                      = addTypeRecord(pp);
        tr1->kind                     = LF_STRUCTURE;
        tr1->LF_Structure.memberCount = (uint16_t) typeStruct->fields.size();
        tr1->LF_Structure.sizeOf      = (uint16_t) typeStruct->sizeOf;
        tr1->LF_Structure.fieldList   = tr0->index;
        tr1->name                     = sname;

        iter.first->second = tr1->index;
        return tr1->index;
    }

    // Enum
    /////////////////////////////////
    if (typeInfo->isEnum())
    {
        TypeInfoEnum* typeInfoEnum = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        auto          sname        = EncoderDebug::getScopedName(typeInfoEnum->declNode);

        // List of values
        if (typeInfoEnum->rawType->isNativeInteger())
        {
            auto tr0  = addTypeRecord(pp);
            tr0->kind = LF_FIELDLIST;
            tr0->LF_FieldList.fields.reserve(typeInfoEnum->values.count);

            VectorNative<TypeInfoEnum*> collect;
            typeInfoEnum->collectEnums(collect);
            for (auto typeEnum : collect)
            {
                for (auto& value : typeEnum->values)
                {
                    if (!value->value)
                        continue;
                    DbgTypeField field;
                    field.kind      = LF_ENUMERATE;
                    field.type      = getOrCreateType(pp, value->typeInfo);
                    field.name      = value->name;
                    field.valueType = typeInfoEnum->rawType;
                    field.value     = *value->value;
                    tr0->LF_FieldList.fields.emplace_back(std::move(field));
                }
            }

            // Enum itself, pointing to the field list
            auto tr1                    = addTypeRecord(pp);
            tr1->kind                   = LF_ENUM;
            tr1->LF_Enum.count          = (uint16_t) typeInfoEnum->values.size();
            tr1->LF_Enum.fieldList      = tr0->index;
            tr1->LF_Enum.underlyingType = getOrCreateType(pp, typeInfoEnum->rawType);
            tr1->name                   = sname;

            iter.first->second = tr1->index;
            return tr1->index;
        }

        else
        {
            return getOrCreateType(pp, typeInfoEnum->rawType);
        }
    }

    // Function
    /////////////////////////////////
    if (typeInfo->isFuncAttr() || typeInfo->isLambdaClosure())
    {
        TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
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
        auto numArgs  = (uint16_t) typeFunc->parameters.size();

        DbgTypeIndex argsTypeIndex;
        using P1                       = MapUtf8<DbgTypeIndex>;
        pair<P1::iterator, bool> iter1 = pp.dbgMapTypesNames.insert(P1::value_type(args, 0));
        if (iter1.second)
        {
            auto tr1              = addTypeRecord(pp);
            tr1->kind             = LF_ARGLIST;
            tr1->LF_ArgList.count = numArgs;
            for (size_t i = 0; i < typeFunc->parameters.size(); i++)
            {
                auto p = typeFunc->parameters[i];
                tr1->LF_ArgList.args.push_back(getOrCreateType(pp, p->typeInfo));
            }

            iter1.first->second = tr1->index;
            argsTypeIndex       = tr1->index;
        }
        else
            argsTypeIndex = iter1.first->second;

        if (isMethod)
        {
            tr0->kind                    = LF_MFUNCTION;
            tr0->LF_MFunction.returnType = getOrCreateType(pp, typeFunc->returnType);
            auto typeThis                = CastTypeInfo<TypeInfoPointer>(typeFunc->parameters[0]->typeInfo, TypeInfoKind::Pointer);
            tr0->LF_MFunction.structType = getOrCreateType(pp, typeThis->pointedType);
            tr0->LF_MFunction.thisType   = getOrCreateType(pp, typeThis);
            tr0->LF_MFunction.numArgs    = numArgs;
            tr0->LF_MFunction.argsType   = argsTypeIndex;
        }
        else
        {
            tr0->kind                    = LF_PROCEDURE;
            tr0->LF_Procedure.returnType = getOrCreateType(pp, typeFunc->returnType);
            tr0->LF_Procedure.numArgs    = numArgs;
            tr0->LF_Procedure.argsType   = argsTypeIndex;
        }

        if (typeInfo->isLambdaClosure())
        {
            auto trp                    = addTypeRecord(pp);
            trp->kind                   = LF_POINTER;
            trp->LF_Pointer.pointeeType = tr0->index;
            iter.first->second          = trp->index;
            return trp->index;
        }

        iter.first->second = tr0->index;
        return tr0->index;
    }

    switch (typeInfo->sizeOf)
    {
    case 1:
        return (DbgTypeIndex) SimpleTypeKind::UnsignedCharacter;
    case 2:
        return (DbgTypeIndex) SimpleTypeKind::UInt16;
    case 4:
        return (DbgTypeIndex) SimpleTypeKind::UInt32;
    default:
        return (DbgTypeIndex) SimpleTypeKind::UInt64;
    }
}

DbgTypeIndex EncoderDebug::getSimpleType(TypeInfo* typeInfo)
{
    if (typeInfo->isNative())
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::Void:
            return SimpleTypeKind::Void;
        case NativeTypeKind::Bool:
            return SimpleTypeKind::Boolean8;
        case NativeTypeKind::S8:
            return SimpleTypeKind::SByte;
        case NativeTypeKind::S16:
            return SimpleTypeKind::Int16;
        case NativeTypeKind::S32:
            return SimpleTypeKind::Int32;
        case NativeTypeKind::S64:
            return SimpleTypeKind::Int64;
        case NativeTypeKind::U8:
            return SimpleTypeKind::Byte;
        case NativeTypeKind::U16:
            return SimpleTypeKind::UInt16;
        case NativeTypeKind::U32:
            return SimpleTypeKind::UInt32;
        case NativeTypeKind::U64:
            return SimpleTypeKind::UInt64;
        case NativeTypeKind::F32:
            return SimpleTypeKind::Float32;
        case NativeTypeKind::F64:
            return SimpleTypeKind::Float64;
        case NativeTypeKind::Rune:
            return SimpleTypeKind::Character32;
        default:
            break;
        }
    }

    return SimpleTypeKind::None;
}

DbgTypeIndex EncoderDebug::getOrCreatePointerToType(EncoderCPU& pp, TypeInfo* typeInfo, bool asRef)
{
    auto simpleType = getSimpleType(typeInfo);
    if (simpleType != SimpleTypeKind::None)
        return (DbgTypeIndex) (simpleType | (NearPointer64 << 8));

    // Pointer to something complex
    auto tr                    = addTypeRecord(pp);
    tr->kind                   = LF_POINTER;
    tr->LF_Pointer.pointeeType = getOrCreateType(pp, typeInfo, !asRef);
    tr->LF_Pointer.asRef       = asRef;
    return tr->index;
}

DbgTypeIndex EncoderDebug::getOrCreatePointerPointerToType(EncoderCPU& pp, TypeInfo* typeInfo)
{
    // Pointer to something complex
    auto tr                    = addTypeRecord(pp);
    tr->kind                   = LF_POINTER;
    tr->LF_Pointer.pointeeType = getOrCreatePointerToType(pp, typeInfo, false);
    return tr->index;
}

DbgTypeRecord* EncoderDebug::addTypeRecord(EncoderCPU& pp)
{
    auto tr   = pp.dbgTypeRecords.addObj<DbgTypeRecord>();
    tr->index = (DbgTypeIndex) pp.dbgTypeRecordsCount + 0x1000;
    pp.dbgTypeRecordsCount++;
    return tr;
}

Utf8 EncoderDebug::getScopedName(AstNode* node)
{
    auto nn = node->getScopedName();
    Utf8 result;
    result.reserve(nn.allocated);

    auto pz      = nn.buffer;
    bool lastDot = false;
    for (uint32_t i = 0; i < nn.length(); i++, pz++)
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

void EncoderDebug::setLocation(CoffFunction* coffFct, ByteCode* bc, ByteCodeInstruction* ip, uint32_t byteOffset)
{
    if (!coffFct->node || coffFct->node->isSpecialFunctionGenerated())
        return;

    if (!ip)
    {
        DbgLine dbgLine;
        dbgLine.line       = coffFct->node->token.startLocation.line + 1;
        dbgLine.byteOffset = byteOffset;
        DbgLines dbgLines;
        dbgLines.sourceFile = coffFct->node->sourceFile;
        dbgLines.lines.push_back(dbgLine);
        coffFct->dbgLines.push_back(dbgLines);
        return;
    }

    if (ip->node->kind == AstNodeKind::FuncDecl)
    {
        return;
    }

    auto loc = ByteCode::getLocation(bc, ip, bc->sourceFile && bc->sourceFile->module->buildCfg.backendDebugInline);
    if (!loc.location)
        return;

    // Update begin of start scope
    auto scope = ip->node->ownerScope;
    while (true)
    {
        if (scope->backendStart == 0)
            scope->backendStart = byteOffset;
        scope->backendEnd = byteOffset;
        if (scope->kind == ScopeKind::Function)
            break;
        scope = scope->parentScope;
    }

    SWAG_ASSERT(!coffFct->dbgLines.empty());

    AstFuncDecl* inlined = nullptr;
    if (ip->node && ip->node->ownerInline)
        inlined = ip->node->ownerInline->func;

    if (coffFct->dbgLines.back().sourceFile != loc.file || inlined != coffFct->dbgLines.back().inlined)
    {
        DbgLines dbgLines;
        dbgLines.sourceFile = loc.file;
        dbgLines.inlined    = inlined;
        coffFct->dbgLines.push_back(dbgLines);
    }

    // Add a new line if it is different from the previous one
    auto& lines = coffFct->dbgLines.back().lines;
    if (lines.empty())
    {
        lines.push_back({loc.location->line + 1, byteOffset});
    }
    else if (lines.back().line != loc.location->line + 1)
    {
        if (lines.back().byteOffset == byteOffset)
            lines.back().line = loc.location->line + 1;
        else
            lines.push_back({loc.location->line + 1, byteOffset});
    }
}

bool EncoderDebug::emit(const BuildParameters& buildParameters, BackendSCBE* scbe, EncoderCPU& pp)
{
    EncoderDebug_CodeView dbg;
    dbg.scbe = scbe;
    return dbg.emit(buildParameters, pp);
}
