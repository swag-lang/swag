#include "pch.h"
#include "SCBE_CodeView.h"
#include "ByteCode.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "SCBE.h"
#include "SCBE_Coff.h"
#include "Scope.h"
#include "TypeManager.h"
#include "Version.h"
#include "Workspace.h"

static void emitStartRecord(SCBE_CPU& pp, uint16_t what)
{
    auto& concat = pp.concat;
    SWAG_ASSERT(pp.dbgRecordIdx < pp.MAX_RECORD);
    pp.dbgStartRecordPtr[pp.dbgRecordIdx]    = concat.addU16Addr(0);
    pp.dbgStartRecordOffset[pp.dbgRecordIdx] = concat.totalCount();
    concat.addU16(what);
    pp.dbgRecordIdx++;
}

static void emitEndRecord(SCBE_CPU& pp, bool align = true)
{
    auto& concat = pp.concat;
    if (align)
        concat.align(4);
    SWAG_ASSERT(pp.dbgRecordIdx);
    pp.dbgRecordIdx--;
    *pp.dbgStartRecordPtr[pp.dbgRecordIdx] = (uint16_t) (concat.totalCount() - pp.dbgStartRecordOffset[pp.dbgRecordIdx]);
}

static void emitCompilerFlagsDebugS(SCBE_CPU& pp)
{
    auto& concat = pp.concat;

    concat.addU32(DEBUG_S_SYMBOLS);
    const auto patchSCount  = concat.addU32Addr(0); // Size of sub section
    const auto patchSOffset = concat.totalCount();

    const auto patchRecordCount  = concat.addU16Addr(0); // Record length, starting from &RecordKind.
    const auto patchRecordOffset = concat.totalCount();
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
    const Utf8 version = FMT("swag %d.%d.%d", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM);
    concat.addString(version.c_str(), version.length() + 1);
    concat.align(4);
    *patchRecordCount = (uint16_t) (concat.totalCount() - patchRecordOffset);

    *patchSCount = concat.totalCount() - patchSOffset;
}

static void emitTruncatedString(SCBE_CPU& pp, const Utf8& str)
{
    auto& concat = pp.concat;
    SWAG_ASSERT(str.length() < 0xF00); // Magic number from llvm codeview debug (should truncate)
    if (str.buffer && str.count)
        concat.addString(str.buffer, str.count);
    concat.addU8(0);
}

static void emitSecRel(SCBE_CPU& pp, uint32_t symbolIndex, uint32_t segIndex, uint32_t offset = 0)
{
    auto& concat = pp.concat;

    // Function symbol index relocation
    CPURelocation reloc;
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

static void emitEmbeddedValue(SCBE_CPU& pp, TypeInfo* valueType, ComputedValue& val)
{
    auto& concat = pp.concat;
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

static bool emitDataDebugT(SCBE_CPU& pp)
{
    auto& concat = pp.concat;

    auto bucket = pp.dbgTypeRecords.firstBucket;
    auto f      = (SCBE_DebugTypeRecord*) bucket->datas;
    int  cpt    = 0;

    while (true)
    {
        emitStartRecord(pp, f->kind);
        switch (f->kind)
        {
        case LF_ARGLIST:
            concat.addU32(f->LF_ArgList.count);
            for (const unsigned int arg : f->LF_ArgList.args)
                concat.addU32(arg);
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
            emitTruncatedString(pp, f->node->token.text);
            break;

        // lfMFuncId
        case LF_MFUNC_ID:
            concat.addU32(f->LF_MFuncId.parentType);
            concat.addU32(f->LF_MFuncId.type);
            emitTruncatedString(pp, f->node->token.text);
            break;

        case LF_ARRAY:
            concat.addU32(f->LF_Array.elementType);
            concat.addU32(f->LF_Array.indexType);
            concat.addU16(LF_ULONG);
            concat.addU32(f->LF_Array.sizeOf);
            emitTruncatedString(pp, "");
            break;

        case LF_DERIVED:
            concat.addU32((uint16_t) f->LF_DerivedList.derived.size());
            for (const auto& p : f->LF_DerivedList.derived)
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
                    emitEmbeddedValue(pp, p.valueType, p.value);
                    break;
                }
                emitTruncatedString(pp, p.name);
            }
            break;

        // https://llvm.org/docs/PDB/CodeViewTypes.html#lf-pointer-0x1002
        case LF_POINTER:
        {
            concat.addU32(f->LF_Pointer.pointeeType);
            constexpr uint32_t kind      = 0x0C; // Near64
            const uint32_t     mode      = f->LF_Pointer.asRef ? CV_PTR_MODE_LVREF : 0;
            constexpr uint32_t modifiers = 0;
            constexpr uint32_t size      = 8; // 64 bits
            constexpr uint32_t flags     = 0;
            const uint32_t     layout    = (flags << 19) | (size << 13) | (modifiers << 8) | (mode << 5) | kind;
            concat.addU32(layout); // attributes
            break;
        }

        case LF_ENUM:
            concat.addU16(f->LF_Enum.count);
            concat.addU16(0); // properties
            concat.addU32(f->LF_Enum.underlyingType);
            concat.addU32(f->LF_Enum.fieldList);
            emitTruncatedString(pp, f->name);
            break;

        case LF_STRUCTURE:
            concat.addU16(f->LF_Structure.memberCount);
            concat.addU16(f->LF_Structure.forward ? 0x80 : 0); // properties
            concat.addU32(f->LF_Structure.fieldList);          // field
            concat.addU32(f->LF_Structure.derivedList);        // derivedFrom
            concat.addU32(0);                                  // vTableShape
            concat.addU16(LF_ULONG);                           // LF_ULONG
            concat.addU32(f->LF_Structure.sizeOf);
            emitTruncatedString(pp, f->name);
            break;
        }

        emitEndRecord(pp, false);

        cpt += sizeof(SCBE_DebugTypeRecord);
        f += 1;

        if (cpt >= (int) pp.dbgTypeRecords.bucketCount(bucket))
        {
            bucket = bucket->nextBucket;
            if (!bucket)
                break;
            cpt = 0;
            f   = (SCBE_DebugTypeRecord*) bucket->datas;
        }
    }

    return true;
}

static void emitConstant(SCBE_CPU& pp, AstNode* node, const Utf8& name)
{
    auto& concat = pp.concat;
    if (node->typeInfo->isNative() && node->typeInfo->sizeOf <= 8)
    {
        emitStartRecord(pp, S_CONSTANT);
        concat.addU32(SCBE_Debug::getOrCreateType(pp, node->typeInfo));
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

        emitTruncatedString(pp, name);
        emitEndRecord(pp);
    }
}

static void emitGlobalDebugS(SCBE_CPU& pp, VectorNative<AstNode*>& gVars, uint32_t segSymIndex)
{
    auto& concat = pp.concat;
    concat.addU32(DEBUG_S_SYMBOLS);
    const auto patchSCount  = concat.addU32Addr(0);
    const auto patchSOffset = concat.totalCount();

    for (const auto& p : gVars)
    {
        // Compile time constant
        if (p->hasComputedValue())
        {
            emitConstant(pp, p, SCBE_Debug::getScopedName(p));
            continue;
        }

        emitStartRecord(pp, S_LDATA32);
        concat.addU32(SCBE_Debug::getOrCreateType(pp, p->typeInfo));

        CPURelocation reloc;

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

        auto nn = SCBE_Debug::getScopedName(p);
        emitTruncatedString(pp, nn);
        emitEndRecord(pp);
    }

    // Fake symbol, because lld linker (since v12) generates a warning if this subsection is empty (wtf)
    if (patchSOffset == concat.totalCount())
    {
        emitStartRecord(pp, S_LDATA32);
        concat.addU32(SCBE_Debug::getOrCreateType(pp, g_TypeMgr->typeInfoBool));
        concat.addU32(0);
        emitTruncatedString(pp, "__fake__");
        concat.addU16(0);
        emitEndRecord(pp);
    }

    *patchSCount = concat.totalCount() - patchSOffset;
}

static uint32_t getFileChecksum(MapPath<uint32_t>& mapFileNames,
                                Vector<uint32_t>&  arrFileNames,
                                Utf8&              stringTable,
                                SourceFile*        sourceFile)
{
    auto checkSymIndex = 0;

    using P = MapPath<uint32_t>;
    const pair<P::iterator, bool> iter = mapFileNames.insert(P::value_type(sourceFile->path, 0));
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

static bool emitLines(SCBE_CPU&          pp,
                      MapPath<uint32_t>& mapFileNames,
                      Vector<uint32_t>&  arrFileNames,
                      Utf8&              stringTable,
                      Concat&            concat,
                      CPUFunction&       f,
                      size_t             idxDbgLines)
{
    const auto& dbgLines   = f.dbgLines[idxDbgLines];
    const auto  sourceFile = dbgLines.sourceFile;
    const auto& lines      = dbgLines.lines;
    concat.addU32(DEBUG_S_LINES);
    const auto patchLTCount  = concat.addU32Addr(0); // Size of sub section
    const auto patchLTOffset = concat.totalCount();

    // Function symbol index relocation
    // Relocate to the first (relative) byte offset of the first line
    // Size is the address of the next subsection start, or the end of the function for the last one
    const auto startByteIndex = lines[0].byteOffset;
    emitSecRel(pp, f.symbolIndex, pp.symCOIndex, lines[0].byteOffset);
    concat.addU16(0); // Flags
    uint32_t endAddress;
    if (idxDbgLines != f.dbgLines.size() - 1)
        endAddress = f.dbgLines[idxDbgLines + 1].lines[0].byteOffset;
    else
        endAddress = f.endAddress - f.startAddress;
    concat.addU32(endAddress - lines[0].byteOffset); // Code size

    // Compute file name index in the checksum table
    const auto checkSymIndex = getFileChecksum(mapFileNames, arrFileNames, stringTable, sourceFile);

    const auto numLines = (uint32_t) lines.size();
    concat.addU32(checkSymIndex);     // File index in checksum buffer (in bytes!)
    concat.addU32(numLines);          // NumLines
    concat.addU32(12 + numLines * 8); // Code size block in bytes (12 + number of lines * 8)
    for (const auto& line : lines)
    {
        concat.addU32(line.byteOffset - startByteIndex); // Offset in bytes from the start of the section
        concat.addU32(line.line);                        // Line number
    }

    *patchLTCount = concat.totalCount() - patchLTOffset;
    return true;
}

static bool emitScope(SCBE_CPU& pp, CPUFunction& f, Scope* scope)
{
    auto& concat = pp.concat;

    // Empty scope
    if (!scope->backendEnd)
        return true;

    // Header
    /////////////////////////////////
    emitStartRecord(pp, S_BLOCK32);
    concat.addU32(0);                                       // Parent = 0;
    concat.addU32(0);                                       // End = 0;
    concat.addU32(scope->backendEnd - scope->backendStart); // CodeSize;
    emitSecRel(pp, f.symbolIndex, pp.symCOIndex, scope->backendStart);
    emitTruncatedString(pp, "");
    emitEndRecord(pp);

    // Local variables marked as global
    /////////////////////////////////
    const auto funcDecl = (AstFuncDecl*) f.node;
    const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(funcDecl->typeInfo, TypeInfoKind::FuncAttr);
    for (const auto localVar : funcDecl->localGlobalVars)
    {
        if (localVar->ownerScope != scope)
            continue;

        const SymbolOverload* overload = localVar->resolvedSymbolOverload;
        const auto            typeInfo = overload->typeInfo;

        SWAG_ASSERT(localVar->attributeFlags & ATTRIBUTE_GLOBAL);

        emitStartRecord(pp, S_LDATA32);
        concat.addU32(SCBE_Debug::getOrCreateType(pp, typeInfo));

        CPURelocation reloc;
        const auto    segSymIndex = overload->flags & OVERLOAD_VAR_BSS ? pp.symBSIndex : pp.symMSIndex;

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

        emitTruncatedString(pp, localVar->token.text);
        emitEndRecord(pp);
    }

    // Local constants
    /////////////////////////////////
    for (const auto localConst : funcDecl->localConstants)
    {
        if (localConst->ownerScope != scope)
            continue;

        emitConstant(pp, localConst, localConst->token.text);
    }

    // Local variables
    /////////////////////////////////
    for (int i = 0; i < (int) f.node->extByteCode()->bc->localVars.size(); i++)
    {
        const auto localVar = f.node->extByteCode()->bc->localVars[i];
        if (localVar->ownerScope != scope)
            continue;

        const SymbolOverload* overload = localVar->resolvedSymbolOverload;
        const auto            typeInfo = overload->typeInfo;

        //////////
        emitStartRecord(pp, S_LOCAL);
        if (overload->flags & OVERLOAD_RETVAL)
            concat.addU32(SCBE_Debug::getOrCreatePointerToType(pp, typeInfo, true)); // Type
        else
            concat.addU32(SCBE_Debug::getOrCreateType(pp, typeInfo)); // Type
        concat.addU16(0);                                             // CV_LVARFLAGS
        emitTruncatedString(pp, localVar->token.text);
        emitEndRecord(pp);

        //////////
        emitStartRecord(pp, S_DEFRANGE_REGISTER_REL);
        concat.addU16(R_RDI);                  // Register
        concat.addU16(0);                      // Flags
        if (overload->flags & OVERLOAD_RETVAL) // Offset to register
            concat.addU32(pp.getParamStackOffset(&f, typeFunc->numParamsRegisters()));
        else
            concat.addU32(overload->computedValue.storageOffset + f.offsetStack);
        emitSecRel(pp, f.symbolIndex, pp.symCOIndex, localVar->ownerScope->backendStart);
        const auto endOffsetVar = localVar->ownerScope->backendEnd == 0 ? f.endAddress : localVar->ownerScope->backendEnd;
        concat.addU16((uint16_t) (endOffsetVar - localVar->ownerScope->backendStart)); // Range
        emitEndRecord(pp);
    }

    // Sub scopes
    // Must be sorted, from first to last. We use the byte index of the first instruction in the block
    /////////////////////////////////
    if (scope->childScopes.size() > 1)
    {
        ranges::sort(scope->childScopes, [](const Scope* n1, const Scope* n2)
        {
            return n1->backendStart < n2->backendStart;
        });
    }

    for (const auto c : scope->childScopes)
        emitScope(pp, f, c);

    // End
    /////////////////////////////////
    emitStartRecord(pp, S_END);
    emitEndRecord(pp);
    return true;
}

static bool emitFctDebugS(SCBE_CPU& pp)
{
    auto& concat = pp.concat;

    MapPath<uint32_t> mapFileNames;
    Vector<uint32_t>  arrFileNames;
    Utf8              stringTable;

    for (auto& f : pp.functions)
    {
        if (!f.node || f.node->isSpecialFunctionGenerated())
            continue;

        const auto decl     = castAst<AstFuncDecl>(f.node, AstNodeKind::FuncDecl);
        const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(decl->typeInfo, TypeInfoKind::FuncAttr);

        // Add a func id type record
        /////////////////////////////////
        const auto tr = SCBE_Debug::addTypeRecord(pp);
        tr->node      = f.node;
        if (typeFunc->isMethod())
        {
            tr->kind                  = LF_MFUNC_ID;
            const auto typeThis       = castTypeInfo<TypeInfoPointer>(typeFunc->parameters[0]->typeInfo, TypeInfoKind::Pointer);
            tr->LF_MFuncId.parentType = SCBE_Debug::getOrCreateType(pp, typeThis->pointedType);
            tr->LF_MFuncId.type       = SCBE_Debug::getOrCreateType(pp, typeFunc);
        }
        else
        {
            tr->kind           = LF_FUNC_ID;
            tr->LF_FuncId.type = SCBE_Debug::getOrCreateType(pp, typeFunc);
        }

        // Symbol
        /////////////////////////////////
        {
            concat.addU32(DEBUG_S_SYMBOLS);
            const auto patchSCount  = concat.addU32Addr(0);
            const auto patchSOffset = concat.totalCount();

            // Proc ID
            // PROCSYM32
            /////////////////////////////////
            emitStartRecord(pp, S_LPROC32_ID);
            concat.addU32(0);                             // Parent = 0
            concat.addU32(0);                             // End = 0
            concat.addU32(0);                             // Next = 0
            concat.addU32(f.endAddress - f.startAddress); // CodeSize = 0
            concat.addU32(0);                             // DbgStart = 0
            concat.addU32(0);                             // DbgEnd = 0
            concat.addU32(tr->index);                     // FuncID type index
            emitSecRel(pp, f.symbolIndex, pp.symCOIndex);
            concat.addU8(0); // ProcSymFlags Flags = ProcSymFlags::None
            auto nn = SCBE_Debug::getScopedName(f.node);
            emitTruncatedString(pp, nn);
            emitEndRecord(pp);

            // Frame Proc
            /////////////////////////////////
            emitStartRecord(pp, S_FRAMEPROC);
            concat.addU32(f.frameSize); // FrameSize
            concat.addU32(0);           // Padding
            concat.addU32(0);           // Offset of padding
            concat.addU32(0);           // Bytes of callee saved registers
            concat.addU32(0);           // Exception handler offset
            concat.addU32(0);           // Exception handler section
            concat.addU32(0);           // Flags (defines frame register)
            emitEndRecord(pp);

            // Capture parameters
            /////////////////////////////////
            if (decl->captureParameters && !(decl->attributeFlags & ATTRIBUTE_COMPILER_FUNC))
            {
                const auto countParams = decl->captureParameters->childs.size();
                for (size_t i = 0; i < countParams; i++)
                {
                    auto       child     = decl->captureParameters->childs[i];
                    const auto typeParam = child->typeInfo;
                    if (child->kind == AstNodeKind::MakePointer)
                        child = child->childs.front();
                    const auto overload = child->resolvedSymbolOverload;
                    if (!typeParam || !overload)
                        continue;

                    SCBE_DebugTypeIndex typeIdx;
                    switch (typeParam->kind)
                    {
                    case TypeInfoKind::Array:
                        typeIdx = SCBE_Debug::getOrCreatePointerToType(pp, typeParam, false);
                        break;
                    default:
                        typeIdx = SCBE_Debug::getOrCreateType(pp, typeParam);
                        break;
                    }

                    //////////
                    emitStartRecord(pp, S_LOCAL);
                    concat.addU32(typeIdx); // Type
                    concat.addU16(0);
                    emitTruncatedString(pp, child->token.text);
                    emitEndRecord(pp);

                    //////////
                    emitStartRecord(pp, S_DEFRANGE_REGISTER_REL);
                    concat.addU16(R_R12); // Register
                    concat.addU16(0);     // Flags
                    concat.addU32(overload->computedValue.storageOffset);
                    emitSecRel(pp, f.symbolIndex, pp.symCOIndex);
                    concat.addU16((uint16_t) (f.endAddress - f.startAddress)); // Range
                    emitEndRecord(pp);
                }
            }

            // Parameters
            /////////////////////////////////
            if (decl->parameters && !(decl->attributeFlags & ATTRIBUTE_COMPILER_FUNC))
            {
                const auto countParams = decl->parameters->childs.size();
                int        regCounter  = 0;
                for (size_t i = 0; i < countParams; i++)
                {
                    const auto child     = decl->parameters->childs[i];
                    const auto typeParam = typeFunc->parameters[i]->typeInfo;

                    SCBE_DebugTypeIndex typeIdx;
                    switch (typeParam->kind)
                    {
                    case TypeInfoKind::Struct:
                        if (CallConv::structParamByValue(typeFunc, typeParam))
                            typeIdx = SCBE_Debug::getOrCreateType(pp, typeParam);
                        else
                            typeIdx = SCBE_Debug::getOrCreatePointerToType(pp, typeParam, true);
                        break;

                    case TypeInfoKind::Pointer:
                    {
                        if (typeParam->isAutoConstPointerRef())
                        {
                            const auto typeRef = TypeManager::concretePtrRefType(typeParam);
                            if (CallConv::structParamByValue(typeFunc, typeRef))
                                typeIdx = SCBE_Debug::getOrCreateType(pp, typeRef);
                            else
                                typeIdx = SCBE_Debug::getOrCreateType(pp, typeParam);
                        }
                        else
                        {
                            typeIdx = SCBE_Debug::getOrCreateType(pp, typeParam);
                        }

                        break;
                    }

                    case TypeInfoKind::Array:
                        typeIdx = SCBE_Debug::getOrCreatePointerToType(pp, typeParam, false);
                        break;

                    default:
                        typeIdx = SCBE_Debug::getOrCreateType(pp, typeParam);
                        break;
                    }

                    //////////
                    emitStartRecord(pp, S_LOCAL);
                    concat.addU32(typeIdx); // Type
                    concat.addU16(1);       // set fIsParam. If not set, callstack signature won't be good.

                    // The real name, in case of 2 registers, will be created below without the 'fIsParam' flag set.
                    // Because i don't know how two deal with those parameters (in fact we have 2 parameters/registers in the calling convention,
                    // but the signature has only one visible parameter).
                    if (typeParam->numRegisters() == 2)
                        emitTruncatedString(pp, "__" + child->token.text);
                    else
                        emitTruncatedString(pp, child->token.text);

                    emitEndRecord(pp);

                    //////////
                    uint32_t offsetStackParam = 0;
                    uint32_t regParam         = regCounter;
                    if (typeFunc->isVariadic() && i != countParams - 1)
                        regParam += 2;
                    else if (typeFunc->isVariadic())
                        regParam = 0;
                    offsetStackParam = pp.getParamStackOffset(&f, regParam);
                    regCounter += typeParam->numRegisters();

                    //////////
                    emitStartRecord(pp, S_DEFRANGE_REGISTER_REL);
                    concat.addU16(R_RDI); // Register
                    concat.addU16(0);     // Flags
                    concat.addU32(offsetStackParam);
                    emitSecRel(pp, f.symbolIndex, pp.symCOIndex);
                    concat.addU16((uint16_t) (f.endAddress - f.startAddress)); // Range
                    emitEndRecord(pp);

                    // If we have 2 registers then we cannot create a symbol flagged as 'parameter' in order to really see it.
                    if (typeParam->numRegisters() == 2)
                    {
                        typeIdx = SCBE_Debug::getOrCreateType(pp, typeParam);

                        //////////
                        emitStartRecord(pp, S_LOCAL);
                        concat.addU32(typeIdx); // Type
                        concat.addU16(0);       // set fIsParam to 0
                        emitTruncatedString(pp, child->token.text);
                        emitEndRecord(pp);

                        //////////
                        emitStartRecord(pp, S_DEFRANGE_REGISTER_REL);
                        concat.addU16(R_RDI); // Register
                        concat.addU16(0);     // Flags
                        concat.addU32(offsetStackParam);
                        emitSecRel(pp, f.symbolIndex, pp.symCOIndex);
                        concat.addU16((uint16_t) (f.endAddress - f.startAddress)); // Range
                        emitEndRecord(pp);
                    }

                    // Codeview seems to need this pointer to be named "this"...
                    // So add it
                    if (typeFunc->isMethod() && child->token.text == g_LangSpec->name_self)
                    {
                        //////////
                        emitStartRecord(pp, S_LOCAL);
                        concat.addU32(typeIdx); // Type
                        concat.addU16(1);       // set fIsParam
                        emitTruncatedString(pp, "this");
                        emitEndRecord(pp);

                        //////////
                        emitStartRecord(pp, S_DEFRANGE_REGISTER_REL);
                        concat.addU16(R_RDI); // Register
                        concat.addU16(0);     // Flags
                        concat.addU32(offsetStackParam);
                        emitSecRel(pp, f.symbolIndex, pp.symCOIndex);
                        concat.addU16((uint16_t) (f.endAddress - f.startAddress)); // Range
                        emitEndRecord(pp);
                    }
                }
            }

            // Lexical blocks
            /////////////////////////////////
            const auto funcDecl = castAst<AstFuncDecl>(f.node, AstNodeKind::FuncDecl);
            emitScope(pp, f, funcDecl->scope);

            // End
            /////////////////////////////////
            emitStartRecord(pp, S_PROC_ID_END);
            emitEndRecord(pp);

            *patchSCount = concat.totalCount() - patchSOffset;
        }

        // Inlineed lines table
        /////////////////////////////////
        {
            for (auto& dbgLines : f.dbgLines)
            {
                if (!dbgLines.inlined)
                    continue;

                concat.addU32(DEBUG_S_INLINEELINES);
                const auto patchLTCount  = concat.addU32Addr(0); // Size of sub section
                const auto patchLTOffset = concat.totalCount();

                constexpr uint32_t CV_INLINEE_SOURCE_LINE_SIGNATURE = 0;
                concat.addU32(CV_INLINEE_SOURCE_LINE_SIGNATURE);

                const auto checkSymIndex = getFileChecksum(mapFileNames, arrFileNames, stringTable, dbgLines.sourceFile);
                const auto typeIdx       = SCBE_Debug::getOrCreateType(pp, dbgLines.inlined->typeInfo);

                for (const auto l : dbgLines.lines)
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
            emitLines(pp, mapFileNames, arrFileNames, stringTable, concat, f, idxDbgLines);
        }
    }

    // File checksum table
    /////////////////////////////////
    concat.addU32(DEBUG_S_FILECHKSMS);
    concat.addU32((int) arrFileNames.size() * 8); // Size of sub section
    for (const auto& p : arrFileNames)
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

bool SCBE_CodeView::emit(const BuildParameters& buildParameters, SCBE_CPU& pp)
{
    auto&      concat = pp.concat;
    const auto module = buildParameters.module;

    // .debug$S
    concat.align(16);
    *pp.patchDBGSOffset = concat.totalCount();
    concat.addU32(DEBUG_SECTION_MAGIC);
    if (buildParameters.buildCfg->backendDebugInformations)
    {
        pp.dbgTypeRecords.init(100 * 1024);
        emitCompilerFlagsDebugS(pp);
        emitGlobalDebugS(pp, module->globalVarsMutable, pp.symMSIndex);
        emitGlobalDebugS(pp, module->globalVarsBss, pp.symBSIndex);
        emitGlobalDebugS(pp, module->globalVarsConstant, pp.symCSIndex);
        emitFctDebugS(pp);
    }
    *pp.patchDBGSCount = concat.totalCount() - *pp.patchDBGSOffset;

    // .debug$T
    concat.align(16);
    *pp.patchDBGTOffset = concat.totalCount();
    concat.addU32(DEBUG_SECTION_MAGIC);
    if (buildParameters.buildCfg->backendDebugInformations)
        emitDataDebugT(pp);
    *pp.patchDBGTCount = concat.totalCount() - *pp.patchDBGTOffset;

    // Reloc table .debug$S
    if (!pp.relocTableDBGSSection.table.empty())
    {
        concat.align(16);
        *pp.patchDBGSSectionRelocTableOffset = concat.totalCount();
        SCBE_Coff::emitRelocationTable(pp.concat, pp.relocTableDBGSSection, pp.patchDBGSSectionFlags, pp.patchDBGSSectionRelocTableCount);
    }

    return true;
}
