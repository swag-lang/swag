#include "pch.h"
#include "BackendSCBE.h"
#include "BackendSCBEDbg_CodeView.h"
#include "ByteCode.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "TypeManager.h"
#include "Version.h"
#include "Workspace.h"

void BackendSCBEDbg_CodeView::dbgEmitCompilerFlagsDebugS(Concat& concat)
{
    concat.addU32(DEBUG_S_SYMBOLS);
    auto patchSCount  = concat.addU32Addr(0); // Size of sub section
    auto patchSOffset = concat.totalCount();

    auto patchRecordCount  = concat.addU16Addr(0); // Record length, starting from &RecordKind.
    auto patchRecordOffset = concat.totalCount();
    concat.addU16(S_COMPILE3); // Record kind enum (SymRecordKind or TypeRecordKind)

    concat.addU32(0);    // Flags/Language (C)
    concat.addU16(0xD0); // CPU Type (X64)

    // Front end version
    concat.addU16(SWAG_BUILD_VERSION);
    concat.addU16(SWAG_BUILD_REVISION);
    concat.addU16(SWAG_BUILD_NUM);
    concat.addU16(0);

    // Backend end version
    concat.addU16(SWAG_BUILD_VERSION);
    concat.addU16(SWAG_BUILD_REVISION);
    concat.addU16(SWAG_BUILD_NUM);
    concat.addU16(0);

    // Compiler version
    Utf8 version = Fmt("swag %d.%d.%d", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM);
    concat.addString(version.c_str(), version.length() + 1);
    concat.align(4);
    *patchRecordCount = (uint16_t) (concat.totalCount() - patchRecordOffset);

    *patchSCount = concat.totalCount() - patchSOffset;
}

void BackendSCBEDbg_CodeView::dbgStartRecord(EncoderCPU& pp, Concat& concat, uint16_t what)
{
    SWAG_ASSERT(pp.dbgRecordIdx < pp.MAX_RECORD);
    pp.dbgStartRecordPtr[pp.dbgRecordIdx]    = concat.addU16Addr(0);
    pp.dbgStartRecordOffset[pp.dbgRecordIdx] = concat.totalCount();
    concat.addU16(what);
    pp.dbgRecordIdx++;
}

void BackendSCBEDbg_CodeView::dbgEndRecord(EncoderCPU& pp, Concat& concat, bool align)
{
    if (align)
        concat.align(4);
    SWAG_ASSERT(pp.dbgRecordIdx);
    pp.dbgRecordIdx--;
    *pp.dbgStartRecordPtr[pp.dbgRecordIdx] = (uint16_t) (concat.totalCount() - pp.dbgStartRecordOffset[pp.dbgRecordIdx]);
}

void BackendSCBEDbg_CodeView::dbgEmitTruncatedString(Concat& concat, const Utf8& str)
{
    SWAG_ASSERT(str.length() < 0xF00); // Magic number from llvm codeviewdebug (should truncate)
    if (str.buffer && str.count)
        concat.addString(str.buffer, str.count);
    concat.addU8(0);
}

void BackendSCBEDbg_CodeView::dbgEmitSecRel(EncoderCPU& pp, Concat& concat, uint32_t symbolIndex, uint32_t segIndex, uint32_t offset)
{
    CoffRelocation reloc;

    // Function symbol index relocation
    reloc.type           = IMAGE_REL_AMD64_SECREL;
    reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
    reloc.symbolIndex    = symbolIndex;
    pp.relocTableDBGSSection.table.push_back(reloc);
    concat.addU32(offset);

    // .text relocation
    reloc.type           = IMAGE_REL_AMD64_SECTION;
    reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
    reloc.symbolIndex    = segIndex;
    pp.relocTableDBGSSection.table.push_back(reloc);
    concat.addU16(0);
}

void BackendSCBEDbg_CodeView::dbgEmitEmbeddedValue(Concat& concat, TypeInfo* valueType, ComputedValue& val)
{
    SWAG_ASSERT(valueType->isNative());
    switch (valueType->nativeType)
    {
    case NativeTypeKind::Bool:
    case NativeTypeKind::U8:
    case NativeTypeKind::S8:
        concat.addU16(LF_CHAR);
        concat.addU8(val.reg.u8);
        break;

    case NativeTypeKind::S16:
        concat.addU16(LF_SHORT);
        concat.addS16(val.reg.s16);
        break;
    case NativeTypeKind::U16:
        concat.addU16(LF_USHORT);
        concat.addU16(val.reg.u16);
        break;

    case NativeTypeKind::S32:
        concat.addU16(LF_LONG);
        concat.addS32(val.reg.s32);
        break;
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        concat.addU16(LF_ULONG);
        concat.addU32(val.reg.u32);
        break;

    case NativeTypeKind::S64:
        concat.addU16(LF_QUADWORD);
        concat.addS64(val.reg.s64);
        break;
    case NativeTypeKind::U64:
        concat.addU16(LF_UQUADWORD);
        concat.addU64(val.reg.u64);
        break;

    case NativeTypeKind::F32:
        concat.addU16(LF_REAL32);
        concat.addF32(val.reg.f32);
        break;
    case NativeTypeKind::F64:
        concat.addU16(LF_REAL64);
        concat.addF64(val.reg.f64);
        break;

    default:
        // Should never happen, but it's better to not assert
        concat.addU16(LF_UQUADWORD);
        concat.addU64(val.reg.u64);
        break;
    }
}

bool BackendSCBEDbg_CodeView::dbgEmitDataDebugT(EncoderCPU& pp)
{
    auto& concat = pp.concat;

    auto bucket = pp.dbgTypeRecords.firstBucket;
    auto f      = (DbgTypeRecord*) bucket->datas;
    int  cpt    = 0;

    while (true)
    {
        dbgStartRecord(pp, concat, f->kind);
        switch (f->kind)
        {
        case LF_ARGLIST:
            concat.addU32(f->LF_ArgList.count);
            for (size_t i = 0; i < f->LF_ArgList.args.size(); i++)
                concat.addU32(f->LF_ArgList.args[i]);
            break;

        // lfProc
        case LF_PROCEDURE:
            concat.addU32(f->LF_Procedure.returnType);
            concat.addU8(0);                         // calling convention
            concat.addU8(0);                         // attributes
            concat.addU16(f->LF_Procedure.numArgs);  // #params
            concat.addU32(f->LF_Procedure.argsType); // @argstype
            break;

        // lfMFunc
        case LF_MFUNCTION:
            concat.addU32(f->LF_MFunction.returnType);
            concat.addU32(f->LF_MFunction.structType);
            concat.addU32(f->LF_MFunction.thisType);
            concat.addU8(0);                         // calling convention
            concat.addU8(0);                         // attributes
            concat.addU16(f->LF_MFunction.numArgs);  // #params
            concat.addU32(f->LF_MFunction.argsType); // @argstype
            concat.addU32(0);                        // thisAdjust
            break;

        // lfFuncId
        case LF_FUNC_ID:
            concat.addU32(0);                 // ParentScope
            concat.addU32(f->LF_FuncId.type); // @type
            dbgEmitTruncatedString(concat, f->node->token.text);
            break;

        // lfMFuncId
        case LF_MFUNC_ID:
            concat.addU32(f->LF_MFuncId.parentType);
            concat.addU32(f->LF_MFuncId.type);
            dbgEmitTruncatedString(concat, f->node->token.text);
            break;

        case LF_ARRAY:
            concat.addU32(f->LF_Array.elementType);
            concat.addU32(f->LF_Array.indexType);
            concat.addU16(LF_ULONG);
            concat.addU32(f->LF_Array.sizeOf);
            dbgEmitTruncatedString(concat, "");
            break;

        case LF_DERIVED:
            concat.addU32((uint16_t) f->LF_DerivedList.derived.size());
            for (auto& p : f->LF_DerivedList.derived)
                concat.addU32(p);
            break;

        case LF_FIELDLIST:
            for (auto& p : f->LF_FieldList.fields)
            {
                concat.addU16(p.kind);
                concat.addU16(0x03); // private = 1, protected = 2, public = 3
                switch (p.kind)
                {
                case LF_MEMBER:
                    concat.addU32(p.type);
                    concat.addU16(LF_ULONG);
                    concat.addU32(p.value.reg.u32);
                    break;
                case LF_ONEMETHOD:
                    concat.addU32(p.type);
                    break;
                case LF_ENUMERATE:
                    dbgEmitEmbeddedValue(concat, p.valueType, p.value);
                    break;
                }
                dbgEmitTruncatedString(concat, p.name);
            }
            break;

        // https://llvm.org/docs/PDB/CodeViewTypes.html#lf-pointer-0x1002
        case LF_POINTER:
        {
            concat.addU32(f->LF_Pointer.pointeeType);
            uint32_t kind      = 0x0C; // Near64
            uint32_t mode      = f->LF_Pointer.asRef ? CV_PTR_MODE_LVREF : 0;
            uint32_t modifiers = 0;
            uint32_t size      = 8; // 64 bits
            uint32_t flags     = 0;
            uint32_t layout    = (flags << 19) | (size << 13) | (modifiers << 8) | (mode << 5) | kind;
            concat.addU32(layout); // attributes
            break;
        }

        case LF_ENUM:
            concat.addU16(f->LF_Enum.count);
            concat.addU16(0); // properties
            concat.addU32(f->LF_Enum.underlyingType);
            concat.addU32(f->LF_Enum.fieldList);
            dbgEmitTruncatedString(concat, f->name);
            break;

        case LF_STRUCTURE:
            concat.addU16(f->LF_Structure.memberCount);
            concat.addU16(f->LF_Structure.forward ? 0x80 : 0); // properties
            concat.addU32(f->LF_Structure.fieldList);          // field
            concat.addU32(f->LF_Structure.derivedList);        // derivedFrom
            concat.addU32(0);                                  // vTableShape
            concat.addU16(LF_ULONG);                           // LF_ULONG
            concat.addU32(f->LF_Structure.sizeOf);
            dbgEmitTruncatedString(concat, f->name);
            break;
        }

        dbgEndRecord(pp, concat, false);

        cpt += sizeof(DbgTypeRecord);
        f += 1;

        if (cpt >= (int) pp.dbgTypeRecords.bucketCount(bucket))
        {
            bucket = bucket->nextBucket;
            if (!bucket)
                break;
            cpt = 0;
            f   = (DbgTypeRecord*) bucket->datas;
        }
    }

    return true;
}

DbgTypeRecord* BackendSCBEDbg_CodeView::dbgAddTypeRecord(EncoderCPU& pp)
{
    auto tr   = pp.dbgTypeRecords.addObj<DbgTypeRecord>();
    tr->index = (DbgTypeIndex) pp.dbgTypeRecordsCount + 0x1000;
    pp.dbgTypeRecordsCount++;
    return tr;
}

DbgTypeIndex BackendSCBEDbg_CodeView::dbgGetSimpleType(TypeInfo* typeInfo)
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

DbgTypeIndex BackendSCBEDbg_CodeView::dbgGetOrCreatePointerToType(EncoderCPU& pp, TypeInfo* typeInfo, bool asRef)
{
    auto simpleType = dbgGetSimpleType(typeInfo);
    if (simpleType != SimpleTypeKind::None)
        return (DbgTypeIndex) (simpleType | (NearPointer64 << 8));

    // Pointer to something complex
    auto tr                    = dbgAddTypeRecord(pp);
    tr->kind                   = LF_POINTER;
    tr->LF_Pointer.pointeeType = dbgGetOrCreateType(pp, typeInfo, !asRef);
    tr->LF_Pointer.asRef       = asRef;
    return tr->index;
}

DbgTypeIndex BackendSCBEDbg_CodeView::dbgGetOrCreatePointerPointerToType(EncoderCPU& pp, TypeInfo* typeInfo)
{
    // Pointer to something complex
    auto tr                    = dbgAddTypeRecord(pp);
    tr->kind                   = LF_POINTER;
    tr->LF_Pointer.pointeeType = dbgGetOrCreatePointerToType(pp, typeInfo, false);
    return tr->index;
}

DbgTypeIndex BackendSCBEDbg_CodeView::dbgEmitTypeSlice(EncoderCPU& pp, TypeInfo* typeInfo, TypeInfo* pointedType, DbgTypeIndex* value)
{
    auto         tr0 = dbgAddTypeRecord(pp);
    DbgTypeField field;
    tr0->kind           = LF_FIELDLIST;
    field.kind          = LF_MEMBER;
    field.type          = dbgGetOrCreatePointerToType(pp, pointedType, false);
    field.value.reg.u32 = 0;
    field.name.setView(g_LangSpec->name_data);
    tr0->LF_FieldList.fields.reserve(2);
    tr0->LF_FieldList.fields.emplace_back(std::move(field));

    field.kind          = LF_MEMBER;
    field.type          = (DbgTypeIndex) (SimpleTypeKind::UInt64);
    field.value.reg.u32 = sizeof(void*);
    field.name.setView(g_LangSpec->name_count);
    tr0->LF_FieldList.fields.emplace_back(std::move(field));

    auto tr1                      = dbgAddTypeRecord(pp);
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

void BackendSCBEDbg_CodeView::dbgRecordFields(EncoderCPU& pp, DbgTypeRecord* tr, TypeInfoStruct* typeStruct, uint32_t baseOffset)
{
    tr->LF_FieldList.fields.reserve(typeStruct->fields.count);
    for (auto& p : typeStruct->fields)
    {
        DbgTypeField field;
        field.kind = LF_MEMBER;
        field.type = dbgGetOrCreateType(pp, p->typeInfo);
        field.name.setView(p->name);
        field.value.reg.u32 = baseOffset + p->offset;
        tr->LF_FieldList.fields.emplace_back(std::move(field));

        if (p->flags & TYPEINFOPARAM_HAS_USING && p->typeInfo->isStruct())
        {
            auto typeStructField = CastTypeInfo<TypeInfoStruct>(p->typeInfo, TypeInfoKind::Struct);
            dbgRecordFields(pp, tr, typeStructField, baseOffset + p->offset);
        }
    }
}

DbgTypeIndex BackendSCBEDbg_CodeView::dbgGetOrCreateType(EncoderCPU& pp, TypeInfo* typeInfo, bool forceUnRef)
{
    typeInfo = typeInfo->getConcreteAlias();

    // Simple type
    auto simpleType = dbgGetSimpleType(typeInfo);
    if (simpleType != SimpleTypeKind::None)
        return simpleType;

    // Pointer
    /////////////////////////////////
    if (typeInfo->isPointer())
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        return dbgGetOrCreatePointerToType(pp, typePtr->pointedType, typePtr->isPointerRef() && !forceUnRef);
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
        return dbgEmitTypeSlice(pp, typeInfo, typeInfoPtr->pointedType, &iter.first->second);
    }

    // TypedVariadic
    /////////////////////////////////
    if (typeInfo->isVariadic())
    {
        return dbgEmitTypeSlice(pp, typeInfo, g_TypeMgr->typeInfoAny, &iter.first->second);
    }

    // TypedVariadic
    /////////////////////////////////
    if (typeInfo->isTypedVariadic())
    {
        auto typeInfoPtr = CastTypeInfo<TypeInfoVariadic>(typeInfo, TypeInfoKind::TypedVariadic);
        return dbgEmitTypeSlice(pp, typeInfo, typeInfoPtr->rawType, &iter.first->second);
    }

    // Static array
    /////////////////////////////////
    if (typeInfo->isArray())
    {
        auto typeArr             = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto tr                  = dbgAddTypeRecord(pp);
        tr->kind                 = LF_ARRAY;
        tr->LF_Array.elementType = dbgGetOrCreateType(pp, typeArr->pointedType, true);
        tr->LF_Array.indexType   = SimpleTypeKind::UInt64;
        tr->LF_Array.sizeOf      = typeArr->sizeOf;
        iter.first->second       = tr->index;
        return tr->index;
    }

    // Native string
    /////////////////////////////////
    if (typeInfo->isString())
    {
        auto         tr0 = dbgAddTypeRecord(pp);
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

        auto tr1                      = dbgAddTypeRecord(pp);
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
        auto         tr0 = dbgAddTypeRecord(pp);
        DbgTypeField field;
        tr0->kind           = LF_FIELDLIST;
        field.kind          = LF_MEMBER;
        field.type          = (DbgTypeIndex) (SimpleTypeKind::UnsignedCharacter | (NearPointer64 << 8));
        field.value.reg.u32 = 0;
        field.name.setView(g_LangSpec->name_data);
        tr0->LF_FieldList.fields.reserve(2);
        tr0->LF_FieldList.fields.emplace_back(std::move(field));

        field.kind          = LF_MEMBER;
        field.type          = dbgGetOrCreatePointerPointerToType(pp, g_Workspace->swagScope.regTypeInfoStruct);
        field.value.reg.u32 = sizeof(void*);
        field.name.setView(g_LangSpec->name_itable);
        tr0->LF_FieldList.fields.emplace_back(std::move(field));

        auto tr1                      = dbgAddTypeRecord(pp);
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
        auto         tr0 = dbgAddTypeRecord(pp);
        DbgTypeField field;
        tr0->kind           = LF_FIELDLIST;
        field.kind          = LF_MEMBER;
        field.type          = (DbgTypeIndex) (SimpleTypeKind::UnsignedCharacter | (NearPointer64 << 8));
        field.value.reg.u32 = 0;
        field.name.setView(g_LangSpec->name_ptrvalue);
        tr0->LF_FieldList.fields.reserve(2);
        tr0->LF_FieldList.fields.emplace_back(std::move(field));

        field.kind          = LF_MEMBER;
        field.type          = dbgGetOrCreatePointerToType(pp, g_Workspace->swagScope.regTypeInfo, false);
        field.value.reg.u32 = sizeof(void*);
        field.name.setView(g_LangSpec->name_typeinfo);
        tr0->LF_FieldList.fields.emplace_back(std::move(field));

        auto tr1                      = dbgAddTypeRecord(pp);
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
        auto            sname      = BackendSCBEDbg::dbgGetScopedName(typeStruct->declNode);

        if (typeStruct->flags & TYPEINFO_FROM_GENERIC)
        {
            auto pz = strchr(typeStruct->name.c_str(), '\'');
            if (pz)
                sname += pz;
        }

        // Create a forward reference, in case a field points to the struct itself
        auto tr2                  = dbgAddTypeRecord(pp);
        tr2->kind                 = LF_STRUCTURE;
        tr2->LF_Structure.forward = true;
        tr2->name                 = sname;

        iter.first->second = tr2->index;

        // List of fields, after the forward ref
        auto tr0  = dbgAddTypeRecord(pp);
        tr0->kind = LF_FIELDLIST;
        dbgRecordFields(pp, tr0, typeStruct, 0);

        tr0->LF_FieldList.fields.reserve(typeStruct->methods.count);
        for (auto& p : typeStruct->methods)
        {
            DbgTypeField field;
            field.kind = LF_ONEMETHOD;
            field.type = dbgGetOrCreateType(pp, p->typeInfo);
            field.name = BackendSCBEDbg::dbgGetScopedName(p->typeInfo->declNode);
            tr0->LF_FieldList.fields.emplace_back(std::move(field));
        }

        // Struct itself, pointing to the field list
        auto tr1                      = dbgAddTypeRecord(pp);
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
        auto          sname        = BackendSCBEDbg::dbgGetScopedName(typeInfoEnum->declNode);

        // List of values
        if (typeInfoEnum->rawType->isNativeInteger())
        {
            auto tr0  = dbgAddTypeRecord(pp);
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
                    field.type      = dbgGetOrCreateType(pp, value->typeInfo);
                    field.name      = value->name;
                    field.valueType = typeInfoEnum->rawType;
                    field.value     = *value->value;
                    tr0->LF_FieldList.fields.emplace_back(std::move(field));
                }
            }

            // Enum itself, pointing to the field list
            auto tr1                    = dbgAddTypeRecord(pp);
            tr1->kind                   = LF_ENUM;
            tr1->LF_Enum.count          = (uint16_t) typeInfoEnum->values.size();
            tr1->LF_Enum.fieldList      = tr0->index;
            tr1->LF_Enum.underlyingType = dbgGetOrCreateType(pp, typeInfoEnum->rawType);
            tr1->name                   = sname;

            iter.first->second = tr1->index;
            return tr1->index;
        }

        else
        {
            return dbgGetOrCreateType(pp, typeInfoEnum->rawType);
        }
    }

    // Function
    /////////////////////////////////
    if (typeInfo->isFuncAttr() || typeInfo->isLambdaClosure())
    {
        TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
        auto              tr0      = dbgAddTypeRecord(pp);

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
            auto tr1              = dbgAddTypeRecord(pp);
            tr1->kind             = LF_ARGLIST;
            tr1->LF_ArgList.count = numArgs;
            for (size_t i = 0; i < typeFunc->parameters.size(); i++)
            {
                auto p = typeFunc->parameters[i];
                tr1->LF_ArgList.args.push_back(dbgGetOrCreateType(pp, p->typeInfo));
            }

            iter1.first->second = tr1->index;
            argsTypeIndex       = tr1->index;
        }
        else
            argsTypeIndex = iter1.first->second;

        if (isMethod)
        {
            tr0->kind                    = LF_MFUNCTION;
            tr0->LF_MFunction.returnType = dbgGetOrCreateType(pp, typeFunc->returnType);
            auto typeThis                = CastTypeInfo<TypeInfoPointer>(typeFunc->parameters[0]->typeInfo, TypeInfoKind::Pointer);
            tr0->LF_MFunction.structType = dbgGetOrCreateType(pp, typeThis->pointedType);
            tr0->LF_MFunction.thisType   = dbgGetOrCreateType(pp, typeThis);
            tr0->LF_MFunction.numArgs    = numArgs;
            tr0->LF_MFunction.argsType   = argsTypeIndex;
        }
        else
        {
            tr0->kind                    = LF_PROCEDURE;
            tr0->LF_Procedure.returnType = dbgGetOrCreateType(pp, typeFunc->returnType);
            tr0->LF_Procedure.numArgs    = numArgs;
            tr0->LF_Procedure.argsType   = argsTypeIndex;
        }

        if (typeInfo->isLambdaClosure())
        {
            auto trp                    = dbgAddTypeRecord(pp);
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

void BackendSCBEDbg_CodeView::dbgEmitConstant(EncoderCPU& pp, Concat& concat, AstNode* node, const Utf8& name)
{
    if (node->typeInfo->isNative() && node->typeInfo->sizeOf <= 8)
    {
        dbgStartRecord(pp, concat, S_CONSTANT);
        concat.addU32(dbgGetOrCreateType(pp, node->typeInfo));
        switch (node->typeInfo->sizeOf)
        {
        case 1:
            concat.addU16(LF_CHAR);
            concat.addU8(node->computedValue->reg.u8);
            break;
        case 2:
            concat.addU16(LF_USHORT);
            concat.addU16(node->computedValue->reg.u16);
            break;
        case 4:
            concat.addU16(LF_ULONG);
            concat.addU32(node->computedValue->reg.u32);
            break;
        case 8:
            concat.addU16(LF_QUADWORD);
            concat.addU64(node->computedValue->reg.u64);
            break;
        }

        dbgEmitTruncatedString(concat, name);
        dbgEndRecord(pp, concat);
    }
}

void BackendSCBEDbg_CodeView::dbgEmitGlobalDebugS(EncoderCPU& pp, Concat& concat, VectorNative<AstNode*>& gVars, uint32_t segSymIndex)
{
    concat.addU32(DEBUG_S_SYMBOLS);
    auto patchSCount  = concat.addU32Addr(0);
    auto patchSOffset = concat.totalCount();

    for (auto& p : gVars)
    {
        // Compile time constant
        if (p->hasComputedValue())
        {
            dbgEmitConstant(pp, concat, p, BackendSCBEDbg::dbgGetScopedName(p));
            continue;
        }

        dbgStartRecord(pp, concat, S_LDATA32);
        concat.addU32(dbgGetOrCreateType(pp, p->typeInfo));

        CoffRelocation reloc;

        // symbol index relocation inside segment
        reloc.type           = IMAGE_REL_AMD64_SECREL;
        reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
        reloc.symbolIndex    = segSymIndex;
        pp.relocTableDBGSSection.table.push_back(reloc);
        concat.addU32(p->resolvedSymbolOverload->computedValue.storageOffset);

        // segment relocation
        reloc.type           = IMAGE_REL_AMD64_SECTION;
        reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
        reloc.symbolIndex    = segSymIndex;
        pp.relocTableDBGSSection.table.push_back(reloc);
        concat.addU16(0);

        auto nn = BackendSCBEDbg::dbgGetScopedName(p);
        dbgEmitTruncatedString(concat, nn);
        dbgEndRecord(pp, concat);
    }

    // Fake symbol, because lld linker (since v12) generates a warning if this subsection is empty (wtf)
    if (patchSOffset == concat.totalCount())
    {
        dbgStartRecord(pp, concat, S_LDATA32);
        concat.addU32(dbgGetOrCreateType(pp, g_TypeMgr->typeInfoBool));
        concat.addU32(0);
        dbgEmitTruncatedString(concat, "__fake__");
        concat.addU16(0);
        dbgEndRecord(pp, concat);
    }

    *patchSCount = concat.totalCount() - patchSOffset;
}

static uint32_t getFileChecksum(MapPath<uint32_t>& mapFileNames,
                                Vector<uint32_t>&  arrFileNames,
                                Utf8&              stringTable,
                                SourceFile*        sourceFile)
{
    auto checkSymIndex = 0;

    using P                      = MapPath<uint32_t>;
    pair<P::iterator, bool> iter = mapFileNames.insert(P::value_type(sourceFile->path, 0));
    if (iter.second)
    {
        checkSymIndex = (uint32_t) arrFileNames.size();
        arrFileNames.push_back((uint32_t) stringTable.length());
        iter.first->second = checkSymIndex;
        stringTable += sourceFile->path.string();
        stringTable.append((char) 0);
    }
    else
    {
        checkSymIndex = iter.first->second;
    }

    return checkSymIndex * 8;
}

bool BackendSCBEDbg_CodeView::dbgEmitLines(EncoderCPU&        pp,
                                           MapPath<uint32_t>& mapFileNames,
                                           Vector<uint32_t>&  arrFileNames,
                                           Utf8&              stringTable,
                                           Concat&            concat,
                                           CoffFunction&      f,
                                           size_t             idxDbgLines)
{
    auto& dbgLines   = f.dbgLines[idxDbgLines];
    auto  sourceFile = dbgLines.sourceFile;
    auto& lines      = dbgLines.lines;
    concat.addU32(DEBUG_S_LINES);
    auto patchLTCount  = concat.addU32Addr(0); // Size of sub section
    auto patchLTOffset = concat.totalCount();

    // Function symbol index relocation
    // Relocate to the first (relative) byte offset of the first line
    // Size is the address of the next subsection start, or the end of the function for the last one
    auto startByteIndex = lines[0].byteOffset;
    dbgEmitSecRel(pp, concat, f.symbolIndex, pp.symCOIndex, lines[0].byteOffset);
    concat.addU16(0); // Flags
    uint32_t endAddress;
    if (idxDbgLines != f.dbgLines.size() - 1)
        endAddress = f.dbgLines[idxDbgLines + 1].lines[0].byteOffset;
    else
        endAddress = f.endAddress - f.startAddress;
    concat.addU32(endAddress - lines[0].byteOffset); // Code size

    // Compute file name index in the checksum table
    auto checkSymIndex = getFileChecksum(mapFileNames, arrFileNames, stringTable, sourceFile);

    auto numLines = (uint32_t) lines.size();
    concat.addU32(checkSymIndex);     // File index in checksum buffer (in bytes!)
    concat.addU32(numLines);          // NumLines
    concat.addU32(12 + numLines * 8); // Code size block in bytes (12 + number of lines * 8)
    for (auto& line : lines)
    {
        concat.addU32(line.byteOffset - startByteIndex); // Offset in bytes from the start of the section
        concat.addU32(line.line);                        // Line number
    }

    *patchLTCount = concat.totalCount() - patchLTOffset;
    return true;
}

bool BackendSCBEDbg_CodeView::dbgEmitFctDebugS(EncoderCPU& pp)
{
    auto& concat = pp.concat;

    MapPath<uint32_t> mapFileNames;
    Vector<uint32_t>  arrFileNames;
    Utf8              stringTable;

    for (auto& f : pp.functions)
    {
        if (!f.node || f.node->isSpecialFunctionGenerated())
            continue;

        auto decl     = CastAst<AstFuncDecl>(f.node, AstNodeKind::FuncDecl);
        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(decl->typeInfo, TypeInfoKind::FuncAttr);

        // Add a func id type record
        /////////////////////////////////
        auto tr  = dbgAddTypeRecord(pp);
        tr->node = f.node;
        if (typeFunc->isMethod())
        {
            tr->kind                  = LF_MFUNC_ID;
            auto typeThis             = CastTypeInfo<TypeInfoPointer>(typeFunc->parameters[0]->typeInfo, TypeInfoKind::Pointer);
            tr->LF_MFuncId.parentType = dbgGetOrCreateType(pp, typeThis->pointedType);
            tr->LF_MFuncId.type       = dbgGetOrCreateType(pp, typeFunc);
        }
        else
        {
            tr->kind           = LF_FUNC_ID;
            tr->LF_FuncId.type = dbgGetOrCreateType(pp, typeFunc);
        }

        // Symbol
        /////////////////////////////////
        {
            concat.addU32(DEBUG_S_SYMBOLS);
            auto patchSCount  = concat.addU32Addr(0);
            auto patchSOffset = concat.totalCount();

            // Proc ID
            // PROCSYM32
            /////////////////////////////////
            dbgStartRecord(pp, concat, S_LPROC32_ID);
            concat.addU32(0);                             // Parent = 0
            concat.addU32(0);                             // End = 0
            concat.addU32(0);                             // Next = 0
            concat.addU32(f.endAddress - f.startAddress); // CodeSize = 0
            concat.addU32(0);                             // DbgStart = 0
            concat.addU32(0);                             // DbgEnd = 0
            concat.addU32(tr->index);                     // FuncID type index
            dbgEmitSecRel(pp, concat, f.symbolIndex, pp.symCOIndex);
            concat.addU8(0); // ProcSymFlags Flags = ProcSymFlags::None
            auto nn = BackendSCBEDbg::dbgGetScopedName(f.node);
            dbgEmitTruncatedString(concat, nn);
            dbgEndRecord(pp, concat);

            // Frame Proc
            /////////////////////////////////
            dbgStartRecord(pp, concat, S_FRAMEPROC);
            concat.addU32(f.frameSize); // FrameSize
            concat.addU32(0);           // Padding
            concat.addU32(0);           // Offset of padding
            concat.addU32(0);           // Bytes of callee saved registers
            concat.addU32(0);           // Exception handler offset
            concat.addU32(0);           // Exception handler section
            concat.addU32(0);           // Flags (defines frame register)
            dbgEndRecord(pp, concat);

            // Capture parameters
            /////////////////////////////////
            if (decl->captureParameters && !(decl->attributeFlags & ATTRIBUTE_COMPILER_FUNC))
            {
                auto countParams = decl->captureParameters->childs.size();
                for (size_t i = 0; i < countParams; i++)
                {
                    auto child     = decl->captureParameters->childs[i];
                    auto typeParam = child->typeInfo;
                    if (child->kind == AstNodeKind::MakePointer)
                        child = child->childs.front();
                    auto overload = child->resolvedSymbolOverload;
                    if (!typeParam || !overload)
                        continue;

                    DbgTypeIndex typeIdx;
                    switch (typeParam->kind)
                    {
                    case TypeInfoKind::Array:
                        typeIdx = dbgGetOrCreatePointerToType(pp, typeParam, false);
                        break;
                    default:
                        typeIdx = dbgGetOrCreateType(pp, typeParam);
                        break;
                    }

                    //////////
                    dbgStartRecord(pp, concat, S_LOCAL);
                    concat.addU32(typeIdx); // Type
                    concat.addU16(0);
                    dbgEmitTruncatedString(concat, child->token.text);
                    dbgEndRecord(pp, concat);

                    //////////
                    dbgStartRecord(pp, concat, S_DEFRANGE_REGISTER_REL);
                    concat.addU16(R_R12); // Register
                    concat.addU16(0);     // Flags
                    concat.addU32(overload->computedValue.storageOffset);
                    dbgEmitSecRel(pp, concat, f.symbolIndex, pp.symCOIndex);
                    concat.addU16((uint16_t) (f.endAddress - f.startAddress)); // Range
                    dbgEndRecord(pp, concat);
                }
            }

            // Parameters
            /////////////////////////////////
            if (decl->parameters && !(decl->attributeFlags & ATTRIBUTE_COMPILER_FUNC))
            {
                auto countParams = decl->parameters->childs.size();
                int  regCounter  = 0;
                for (size_t i = 0; i < countParams; i++)
                {
                    auto child     = decl->parameters->childs[i];
                    auto typeParam = typeFunc->parameters[i]->typeInfo;

                    DbgTypeIndex typeIdx;
                    switch (typeParam->kind)
                    {
                    case TypeInfoKind::Struct:
                        if (CallConv::structParamByValue(typeFunc, typeParam))
                            typeIdx = dbgGetOrCreateType(pp, typeParam);
                        else
                            typeIdx = dbgGetOrCreatePointerToType(pp, typeParam, true);
                        break;

                    case TypeInfoKind::Pointer:
                    {
                        if (typeParam->isAutoConstPointerRef())
                        {
                            auto typeRef = TypeManager::concretePtrRefType(typeParam);
                            if (CallConv::structParamByValue(typeFunc, typeRef))
                                typeIdx = dbgGetOrCreateType(pp, typeRef);
                            else
                                typeIdx = dbgGetOrCreateType(pp, typeParam);
                        }
                        else
                        {
                            typeIdx = dbgGetOrCreateType(pp, typeParam);
                        }

                        break;
                    }

                    case TypeInfoKind::Array:
                        typeIdx = dbgGetOrCreatePointerToType(pp, typeParam, false);
                        break;

                    default:
                        typeIdx = dbgGetOrCreateType(pp, typeParam);
                        break;
                    }

                    //////////
                    dbgStartRecord(pp, concat, S_LOCAL);
                    concat.addU32(typeIdx); // Type
                    concat.addU16(1);       // set fIsParam. If not set, callstack signature won't be good.

                    // The real name, in case of 2 registers, will be created below without the 'fIsParam' flag set.
                    // Because i don't know how two deal with those parameters (in fact we have 2 parameters/registers in the calling convention,
                    // but the signature has only one visible parameter).
                    if (typeParam->numRegisters() == 2)
                        dbgEmitTruncatedString(concat, "__" + child->token.text);
                    else
                        dbgEmitTruncatedString(concat, child->token.text);

                    dbgEndRecord(pp, concat);

                    //////////
                    uint32_t offsetStackParam = 0;
                    uint32_t regParam         = regCounter;
                    if (typeFunc->isVariadic() && i != countParams - 1)
                        regParam += 2;
                    else if (typeFunc->isVariadic())
                        regParam = 0;
                    offsetStackParam = scbe->getParamStackOffset(&f, regParam);
                    regCounter += typeParam->numRegisters();

                    //////////
                    dbgStartRecord(pp, concat, S_DEFRANGE_REGISTER_REL);
                    concat.addU16(R_RDI); // Register
                    concat.addU16(0);     // Flags
                    concat.addU32(offsetStackParam);
                    dbgEmitSecRel(pp, concat, f.symbolIndex, pp.symCOIndex);
                    concat.addU16((uint16_t) (f.endAddress - f.startAddress)); // Range
                    dbgEndRecord(pp, concat);

                    // If we have 2 registers then we cannot create a symbol flagged as 'parameter' in order to really see it.
                    if (typeParam->numRegisters() == 2)
                    {
                        typeIdx = dbgGetOrCreateType(pp, typeParam);

                        //////////
                        dbgStartRecord(pp, concat, S_LOCAL);
                        concat.addU32(typeIdx); // Type
                        concat.addU16(0);       // set fIsParam to 0
                        dbgEmitTruncatedString(concat, child->token.text);
                        dbgEndRecord(pp, concat);

                        //////////
                        dbgStartRecord(pp, concat, S_DEFRANGE_REGISTER_REL);
                        concat.addU16(R_RDI); // Register
                        concat.addU16(0);     // Flags
                        concat.addU32(offsetStackParam);
                        dbgEmitSecRel(pp, concat, f.symbolIndex, pp.symCOIndex);
                        concat.addU16((uint16_t) (f.endAddress - f.startAddress)); // Range
                        dbgEndRecord(pp, concat);
                    }

                    // Codeview seems to need this pointer to be named "this"...
                    // So add it
                    if (typeFunc->isMethod() && child->token.text == g_LangSpec->name_self)
                    {
                        //////////
                        dbgStartRecord(pp, concat, S_LOCAL);
                        concat.addU32(typeIdx); // Type
                        concat.addU16(1);       // set fIsParam
                        dbgEmitTruncatedString(concat, "this");
                        dbgEndRecord(pp, concat);

                        //////////
                        dbgStartRecord(pp, concat, S_DEFRANGE_REGISTER_REL);
                        concat.addU16(R_RDI); // Register
                        concat.addU16(0);     // Flags
                        concat.addU32(offsetStackParam);
                        dbgEmitSecRel(pp, concat, f.symbolIndex, pp.symCOIndex);
                        concat.addU16((uint16_t) (f.endAddress - f.startAddress)); // Range
                        dbgEndRecord(pp, concat);
                    }
                }
            }

            // Lexical blocks
            /////////////////////////////////
            auto funcDecl = CastAst<AstFuncDecl>(f.node, AstNodeKind::FuncDecl);
            dbgEmitScope(pp, concat, f, funcDecl->scope);

            // End
            /////////////////////////////////
            dbgStartRecord(pp, concat, S_PROC_ID_END);
            dbgEndRecord(pp, concat);

            *patchSCount = concat.totalCount() - patchSOffset;
        }

        // Inlineed lines table
        /////////////////////////////////
        {
            for (size_t idxDbgLines = 0; idxDbgLines < f.dbgLines.size(); idxDbgLines++)
            {
                auto& dbgLines = f.dbgLines[idxDbgLines];
                if (!dbgLines.inlined)
                    continue;

                concat.addU32(DEBUG_S_INLINEELINES);
                auto patchLTCount  = concat.addU32Addr(0); // Size of sub section
                auto patchLTOffset = concat.totalCount();

                const uint32_t CV_INLINEE_SOURCE_LINE_SIGNATURE = 0;
                concat.addU32(CV_INLINEE_SOURCE_LINE_SIGNATURE);

                auto checkSymIndex = getFileChecksum(mapFileNames, arrFileNames, stringTable, dbgLines.sourceFile);
                auto typeIdx       = dbgGetOrCreateType(pp, dbgLines.inlined->typeInfo);

                for (auto l : dbgLines.lines)
                {
                    concat.addU32(typeIdx);
                    concat.addU32(checkSymIndex);
                    concat.addU32(l.line);
                }

                *patchLTCount = concat.totalCount() - patchLTOffset;
            }
        }

        // Lines table
        /////////////////////////////////
        for (size_t idxDbgLines = 0; idxDbgLines < f.dbgLines.size(); idxDbgLines++)
        {
            dbgEmitLines(pp, mapFileNames, arrFileNames, stringTable, concat, f, idxDbgLines);
        }
    }

    // File checksum table
    /////////////////////////////////
    concat.addU32(DEBUG_S_FILECHKSMS);
    concat.addU32((int) arrFileNames.size() * 8); // Size of sub section
    for (auto& p : arrFileNames)
    {
        concat.addU32(p); // Offset of file name in string table
        concat.addU32(0);
    }

    // String table
    /////////////////////////////////
    concat.addU32(DEBUG_S_STRINGTABLE);
    while (stringTable.length() & 3) // Align to 4 bytes
        stringTable.append((char) 0);
    concat.addU32(stringTable.length());
    concat.addString(stringTable);

    return true;
}

bool BackendSCBEDbg_CodeView::dbgEmitScope(EncoderCPU& pp, Concat& concat, CoffFunction& f, Scope* scope)
{
    // Empty scope
    if (!scope->backendEnd)
        return true;

    // Header
    /////////////////////////////////
    dbgStartRecord(pp, concat, S_BLOCK32);
    concat.addU32(0);                                       // Parent = 0;
    concat.addU32(0);                                       // End = 0;
    concat.addU32(scope->backendEnd - scope->backendStart); // CodeSize;
    dbgEmitSecRel(pp, concat, f.symbolIndex, pp.symCOIndex, scope->backendStart);
    dbgEmitTruncatedString(concat, "");
    dbgEndRecord(pp, concat);

    // Local variables marked as global
    /////////////////////////////////
    auto funcDecl = (AstFuncDecl*) f.node;
    auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(funcDecl->typeInfo, TypeInfoKind::FuncAttr);
    for (int i = 0; i < (int) funcDecl->localGlobalVars.size(); i++)
    {
        auto localVar = funcDecl->localGlobalVars[i];
        if (localVar->ownerScope != scope)
            continue;

        SymbolOverload* overload = localVar->resolvedSymbolOverload;
        auto            typeInfo = overload->typeInfo;

        SWAG_ASSERT(localVar->attributeFlags & ATTRIBUTE_GLOBAL);

        dbgStartRecord(pp, concat, S_LDATA32);
        concat.addU32(dbgGetOrCreateType(pp, typeInfo));

        CoffRelocation reloc;
        auto           segSymIndex = overload->flags & OVERLOAD_VAR_BSS ? pp.symBSIndex : pp.symMSIndex;

        // symbol index relocation inside segment
        reloc.type           = IMAGE_REL_AMD64_SECREL;
        reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
        reloc.symbolIndex    = segSymIndex;
        pp.relocTableDBGSSection.table.push_back(reloc);
        concat.addU32(overload->computedValue.storageOffset);

        // segment relocation
        reloc.type           = IMAGE_REL_AMD64_SECTION;
        reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
        reloc.symbolIndex    = segSymIndex;
        pp.relocTableDBGSSection.table.push_back(reloc);
        concat.addU16(0);

        dbgEmitTruncatedString(concat, localVar->token.text);
        dbgEndRecord(pp, concat);
    }

    // Local constants
    /////////////////////////////////
    for (int i = 0; i < (int) funcDecl->localConstants.size(); i++)
    {
        auto localConst = funcDecl->localConstants[i];
        if (localConst->ownerScope != scope)
            continue;

        dbgEmitConstant(pp, concat, localConst, localConst->token.text);
    }

    // Local variables
    /////////////////////////////////
    for (int i = 0; i < (int) f.node->extByteCode()->bc->localVars.size(); i++)
    {
        auto localVar = f.node->extByteCode()->bc->localVars[i];
        if (localVar->ownerScope != scope)
            continue;

        SymbolOverload* overload = localVar->resolvedSymbolOverload;
        auto            typeInfo = overload->typeInfo;

        //////////
        dbgStartRecord(pp, concat, S_LOCAL);
        if (overload->flags & OVERLOAD_RETVAL)
            concat.addU32(dbgGetOrCreatePointerToType(pp, typeInfo, true)); // Type
        else
            concat.addU32(dbgGetOrCreateType(pp, typeInfo)); // Type
        concat.addU16(0);                                    // CV_LVARFLAGS
        dbgEmitTruncatedString(concat, localVar->token.text);
        dbgEndRecord(pp, concat);

        //////////
        dbgStartRecord(pp, concat, S_DEFRANGE_REGISTER_REL);
        concat.addU16(R_RDI);                  // Register
        concat.addU16(0);                      // Flags
        if (overload->flags & OVERLOAD_RETVAL) // Offset to register
            concat.addU32(scbe->getParamStackOffset(&f, typeFunc->numParamsRegisters()));
        else
            concat.addU32(overload->computedValue.storageOffset + f.offsetStack);
        dbgEmitSecRel(pp, concat, f.symbolIndex, pp.symCOIndex, localVar->ownerScope->backendStart);
        auto endOffsetVar = localVar->ownerScope->backendEnd == 0 ? f.endAddress : localVar->ownerScope->backendEnd;
        concat.addU16((uint16_t) (endOffsetVar - localVar->ownerScope->backendStart)); // Range
        dbgEndRecord(pp, concat);
    }

    // Sub scopes
    // Must be sorted, from first to last. We use the byte index of the first instruction in the block
    /////////////////////////////////
    if (scope->childScopes.size() > 1)
    {
        sort(scope->childScopes.begin(), scope->childScopes.end(), [](Scope* n1, Scope* n2)
             { return n1->backendStart < n2->backendStart; });
    }

    for (auto c : scope->childScopes)
        dbgEmitScope(pp, concat, f, c);

    // End
    /////////////////////////////////
    dbgStartRecord(pp, concat, S_END);
    dbgEndRecord(pp, concat);
    return true;
}

bool BackendSCBEDbg_CodeView::dbgEmit(const BuildParameters& buildParameters, EncoderCPU& pp)
{
    auto& concat = pp.concat;

    // .debug$S
    concat.align(16);
    *pp.patchDBGSOffset = concat.totalCount();
    concat.addU32(DEBUG_SECTION_MAGIC);
    if (buildParameters.buildCfg->backendDebugInformations)
    {
        pp.dbgTypeRecords.init(100 * 1024);
        dbgEmitCompilerFlagsDebugS(concat);
        dbgEmitGlobalDebugS(pp, concat, scbe->module->globalVarsMutable, pp.symMSIndex);
        dbgEmitGlobalDebugS(pp, concat, scbe->module->globalVarsBss, pp.symBSIndex);
        dbgEmitGlobalDebugS(pp, concat, scbe->module->globalVarsConstant, pp.symCSIndex);
        dbgEmitFctDebugS(pp);
    }
    *pp.patchDBGSCount = concat.totalCount() - *pp.patchDBGSOffset;

    // .debug$T
    concat.align(16);
    *pp.patchDBGTOffset = concat.totalCount();
    concat.addU32(DEBUG_SECTION_MAGIC);
    if (buildParameters.buildCfg->backendDebugInformations)
    {
        dbgEmitDataDebugT(pp);
    }
    *pp.patchDBGTCount = concat.totalCount() - *pp.patchDBGTOffset;

    // Reloc table .debug$S
    if (!pp.relocTableDBGSSection.table.empty())
    {
        concat.align(16);
        *pp.patchDBGSSectionRelocTableOffset = concat.totalCount();
        scbe->emitRelocationTable(pp.concat, pp.relocTableDBGSSection, pp.patchDBGSSectionFlags, pp.patchDBGSSectionRelocTableCount);
    }

    return true;
}
