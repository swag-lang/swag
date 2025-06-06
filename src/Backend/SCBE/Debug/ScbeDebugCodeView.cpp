#include "pch.h"
#include "Backend/SCBE/Debug/ScbeDebugCodeView.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/SCBE/Main/Scbe.h"
#include "Backend/SCBE/Obj/ScbeCoff.h"
#include "Main/Version.h"
#include "Semantic/Scope.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

namespace
{
    void emitStartRecord(ScbeCpu& pp, uint16_t what)
    {
        auto& concat = pp.concat;
        SWAG_ASSERT(pp.dbgRecordIdx < pp.MAX_RECORD);
        pp.dbgStartRecordPtr[pp.dbgRecordIdx]    = concat.addU16Addr(0);
        pp.dbgStartRecordOffset[pp.dbgRecordIdx] = concat.totalCount();
        concat.addU16(what);
        pp.dbgRecordIdx++;
    }

    void emitEndRecord(ScbeCpu& pp, bool align = true)
    {
        auto& concat = pp.concat;
        if (align)
            concat.align(4);
        SWAG_ASSERT(pp.dbgRecordIdx);
        pp.dbgRecordIdx--;
        *pp.dbgStartRecordPtr[pp.dbgRecordIdx] = static_cast<uint16_t>(concat.totalCount() - pp.dbgStartRecordOffset[pp.dbgRecordIdx]);
    }

    void emitCompilerFlagsDebugS(ScbeCpu& pp)
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
        const Utf8 version = form("swag %d.%d.%d", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM);
        concat.addStringN(version, version.length() + 1);
        concat.align(4);
        *patchRecordCount = static_cast<uint16_t>(concat.totalCount() - patchRecordOffset);

        *patchSCount = concat.totalCount() - patchSOffset;
    }

    void emitTruncatedString(ScbeCpu& pp, const Utf8& str)
    {
        auto& concat = pp.concat;
        SWAG_ASSERT(str.length() < 0xF00); // Magic number from llvm codeview debug (should truncate)
        if (str.buffer && str.count)
            concat.addStringN(str.buffer, str.count);
        concat.addU8(0);
    }

    void emitSecRel(ScbeCpu& pp, uint32_t symbolIndex, uint32_t segIndex, uint32_t offset = 0)
    {
        auto& concat = pp.concat;

        // Function symbol index relocation
        CpuRelocation reloc;
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

    void emitEmbeddedValue(ScbeCpu& pp, const TypeInfo* valueType, const ComputedValue& val)
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

    bool emitDataDebugT(ScbeCpu& pp)
    {
        auto& concat = pp.concat;

        auto     bucket = pp.dbgTypeRecords.firstBucket;
        auto     f      = reinterpret_cast<ScbeDebugTypeRecord*>(bucket->data);
        uint32_t cpt    = 0;

        while (true)
        {
            emitStartRecord(pp, f->kind);
            switch (f->kind)
            {
                case LF_ARGLIST:
                    concat.addU32(f->lfArgList.count);
                    for (const unsigned int arg : f->lfArgList.args)
                        concat.addU32(arg);
                    break;

                // lfProc
                case LF_PROCEDURE:
                    concat.addU32(f->lfProcedure.returnType);
                    concat.addU8(0); // calling convention
                    concat.addU8(0); // attributes
                    concat.addU16(f->lfProcedure.numArgs);
                    concat.addU32(f->lfProcedure.argsType);
                    break;

                // lfMFunc
                case LF_MFUNCTION:
                    concat.addU32(f->lfMFunction.returnType);
                    concat.addU32(f->lfMFunction.structType);
                    concat.addU32(f->lfMFunction.thisType);
                    concat.addU8(0); // calling convention
                    concat.addU8(0); // attributes
                    concat.addU16(f->lfMFunction.numArgs);
                    concat.addU32(f->lfMFunction.argsType);
                    concat.addU32(0); // thisAdjust
                    break;

                // lfFuncId
                case LF_FUNC_ID:
                    concat.addU32(0);                // ParentScope
                    concat.addU32(f->lfFuncId.type); // @type
                    emitTruncatedString(pp, f->node->token.text);
                    break;

                // lfMFuncId
                case LF_MFUNC_ID:
                    concat.addU32(f->lfMFuncId.parentType);
                    concat.addU32(f->lfMFuncId.type);
                    emitTruncatedString(pp, f->node->token.text);
                    break;

                case LF_ARRAY:
                    concat.addU32(f->lfArray.elementType);
                    concat.addU32(f->lfArray.indexType);
                    concat.addU16(LF_ULONG);
                    concat.addU32(f->lfArray.sizeOf);
                    emitTruncatedString(pp, "");
                    break;

                case LF_DERIVED:
                    concat.addU32(static_cast<uint16_t>(f->lfDerivedList.derived.size()));
                    for (const auto& p : f->lfDerivedList.derived)
                        concat.addU32(p);
                    break;

                case LF_FIELDLIST:
                    for (auto& p : f->lfFieldList.fields)
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
                            default:
                                break;
                        }
                        emitTruncatedString(pp, p.name);
                    }
                    break;

                // https://llvm.org/docs/PDB/CodeViewTypes.html#lf-pointer-0x1002
                case LF_POINTER:
                {
                    concat.addU32(f->lfPointer.pointeeType);
                    constexpr uint32_t kind      = 0x0C; // Near64
                    const uint32_t     mode      = f->lfPointer.asRef ? CV_PTR_MODE_LVREF : 0;
                    constexpr uint32_t modifiers = 0;
                    constexpr uint32_t size      = 8; // 64 bits
                    constexpr uint32_t flags     = 0;
                    const uint32_t     layout    = flags << 19 | size << 13 | modifiers << 8 | mode << 5 | kind;
                    concat.addU32(layout); // attributes
                    break;
                }

                case LF_ENUM:
                    concat.addU16(f->lfEnum.count);
                    concat.addU16(0); // properties
                    concat.addU32(f->lfEnum.underlyingType);
                    concat.addU32(f->lfEnum.fieldList);
                    emitTruncatedString(pp, f->name);
                    break;

                case LF_STRUCTURE:
                    concat.addU16(f->lfStructure.memberCount);
                    concat.addU16(f->lfStructure.forward ? 0x80 : 0); // properties
                    concat.addU32(f->lfStructure.fieldList);          // field
                    concat.addU32(f->lfStructure.derivedList);        // derivedFrom
                    concat.addU32(0);                                 // vTableShape
                    concat.addU16(LF_ULONG);                          // LF_ULONG
                    concat.addU32(f->lfStructure.sizeOf);
                    emitTruncatedString(pp, f->name);
                    break;
                default:
                    break;
            }

            emitEndRecord(pp, false);

            cpt += sizeof(ScbeDebugTypeRecord);
            f += 1;

            if (cpt >= pp.dbgTypeRecords.bucketCount(bucket))
            {
                bucket = bucket->nextBucket;
                if (!bucket)
                    break;
                cpt = 0;
                f   = reinterpret_cast<ScbeDebugTypeRecord*>(bucket->data);
            }
        }

        return true;
    }

    void emitConstant(ScbeCpu& pp, const AstNode* node, const Utf8& name)
    {
        auto& concat = pp.concat;
        if (node->typeInfo->isNative() && node->typeInfo->sizeOf <= 8)
        {
            emitStartRecord(pp, S_CONSTANT);
            concat.addU32(ScbeDebug::getOrCreateType(pp, node->typeInfo));
            switch (node->typeInfo->sizeOf)
            {
                case 1:
                    concat.addU16(LF_CHAR);
                    concat.addU8(node->computedValue()->reg.u8);
                    break;
                case 2:
                    concat.addU16(LF_USHORT);
                    concat.addU16(node->computedValue()->reg.u16);
                    break;
                case 4:
                    concat.addU16(LF_ULONG);
                    concat.addU32(node->computedValue()->reg.u32);
                    break;
                case 8:
                    concat.addU16(LF_QUADWORD);
                    concat.addU64(node->computedValue()->reg.u64);
                    break;
                default:
                    break;
            }

            emitTruncatedString(pp, name);
            emitEndRecord(pp);
        }
    }

    void emitGlobalDebugS(ScbeCpu& pp, VectorNative<AstNode*>& gVars, uint32_t segSymIndex)
    {
        auto& concat = pp.concat;
        concat.addU32(DEBUG_S_SYMBOLS);
        const auto patchSCount  = concat.addU32Addr(0);
        const auto patchSOffset = concat.totalCount();

        for (const auto& p : gVars)
        {
            // compile-time constant
            if (p->hasFlagComputedValue())
            {
                emitConstant(pp, p, ScbeDebug::getScopedName(p));
                continue;
            }

            emitStartRecord(pp, S_LDATA32);
            concat.addU32(ScbeDebug::getOrCreateType(pp, p->typeInfo));

            CpuRelocation reloc;

            // symbol index relocation inside segment
            reloc.type           = IMAGE_REL_AMD64_SECREL;
            reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
            reloc.symbolIndex    = segSymIndex;
            pp.relocTableDBGSSection.table.push_back(reloc);
            concat.addU32(p->resolvedSymbolOverload()->computedValue.storageOffset);

            // segment relocation
            reloc.type           = IMAGE_REL_AMD64_SECTION;
            reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
            reloc.symbolIndex    = segSymIndex;
            pp.relocTableDBGSSection.table.push_back(reloc);
            concat.addU16(0);

            auto scopedName = ScbeDebug::getScopedName(p);
            emitTruncatedString(pp, scopedName);
            emitEndRecord(pp);
        }

        // Fake symbol, because lld linker (since v12) generates a warning if this subsection is empty (wtf)
        if (patchSOffset == concat.totalCount())
        {
            emitStartRecord(pp, S_LDATA32);
            concat.addU32(ScbeDebug::getOrCreateType(pp, g_TypeMgr->typeInfoBool));
            concat.addU32(0);
            emitTruncatedString(pp, "__fake__");
            concat.addU16(0);
            emitEndRecord(pp);
        }

        *patchSCount = concat.totalCount() - patchSOffset;
    }

    uint32_t getFileChecksum(MapPath<uint32_t>& mapFileNames, Vector<uint32_t>& arrFileNames, Utf8& stringTable, SourceFile* sourceFile)
    {
        uint32_t checkSymIndex = 0;

        using P                                 = MapPath<uint32_t>;
        const std::pair<P::iterator, bool> iter = mapFileNames.insert(P::value_type(sourceFile->path, 0));
        if (iter.second)
        {
            checkSymIndex = arrFileNames.size();
            arrFileNames.push_back(stringTable.length());
            iter.first->second = checkSymIndex;
            stringTable += sourceFile->path;
            stringTable.append(static_cast<char>(0));
        }
        else
        {
            checkSymIndex = iter.first->second;
        }

        return checkSymIndex * 8;
    }

    bool emitLines(ScbeCpu& pp, MapPath<uint32_t>& mapFileNames, Vector<uint32_t>& arrFileNames, Utf8& stringTable, Concat& concat, const CpuFunction* f, uint32_t idxDbgLines)
    {
        const auto& dbgLines   = f->dbgLines[idxDbgLines];
        const auto  sourceFile = dbgLines.sourceFile;
        const auto& lines      = dbgLines.lines;
        concat.addU32(DEBUG_S_LINES);
        const auto patchLTCount  = concat.addU32Addr(0); // Size of sub section
        const auto patchLTOffset = concat.totalCount();

        // Function symbol index relocation
        // Relocate to the first (relative) byte offset of the first line
        // Size is the address of the next subsection start, or the end of the function for the last one
        const auto startByteIndex = lines[0].byteOffset;
        emitSecRel(pp, f->symbolIndex, pp.symCOIndex, lines[0].byteOffset);
        concat.addU16(0); // Flags
        uint32_t endAddress;
        if (idxDbgLines != f->dbgLines.size() - 1)
            endAddress = f->dbgLines[idxDbgLines + 1].lines[0].byteOffset;
        else
            endAddress = f->endAddress - f->startAddress;
        concat.addU32(endAddress - lines[0].byteOffset); // Code size

        // Compute file name index in the checksum table
        const auto checkSymIndex = getFileChecksum(mapFileNames, arrFileNames, stringTable, sourceFile);

        const auto numLines = lines.size();
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

    void emitLocalGlobalVars(ScbeCpu& pp, const Scope* scope, Concat& concat, AstFuncDecl* const funcDecl)
    {
        for (const auto localVar : funcDecl->localGlobalVars)
        {
            if (localVar->ownerScope != scope)
                continue;

            const SymbolOverload* overload = localVar->resolvedSymbolOverload();
            const auto            typeInfo = overload->typeInfo;

            SWAG_ASSERT(localVar->hasAttribute(ATTRIBUTE_GLOBAL));

            emitStartRecord(pp, S_LDATA32);
            concat.addU32(ScbeDebug::getOrCreateType(pp, typeInfo));

            CpuRelocation reloc;
            const auto    segSymIndex = overload->hasFlag(OVERLOAD_VAR_BSS) ? pp.symBSIndex : pp.symMSIndex;

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
    }

    void emitLocalVars(ScbeCpu& pp, const CpuFunction* f, const Scope* scope, const TypeInfoFuncAttr* typeFunc)
    {
        auto& concat = pp.concat;
        for (const auto localVar : scope->symTable.allSymbols)
        {
            if (localVar->overloads.empty())
                continue;
            const SymbolOverload* overload = localVar->overloads[0];
            if (overload->hasFlag(OVERLOAD_VAR_FUNC_PARAM))
                continue;

            const auto typeInfo = overload->typeInfo;

            //////////
            emitStartRecord(pp, S_LOCAL);
            if (overload->hasFlag(OVERLOAD_RETVAL))
                concat.addU32(ScbeDebug::getOrCreatePointerToType(pp, typeInfo, true)); // Type
            else
                concat.addU32(ScbeDebug::getOrCreateType(pp, typeInfo)); // Type
            CV_LVARFLAGS cvf = {};
            concat.addU16(std::bit_cast<uint16_t>(cvf));
            emitTruncatedString(pp, localVar->name);
            emitEndRecord(pp);

            emitStartRecord(pp, S_DEFRANGE_REGISTER_REL);
            concat.addU16(R_RSP);                   // Register
            concat.addU16(0);                       // Flags
            if (overload->hasFlag(OVERLOAD_RETVAL)) // Offset to register
                concat.addU32(f->getStackOffsetParam(typeFunc->numParamsRegisters()));
            else
                concat.addU32(f->getStackOffsetBC() + overload->computedValue.storageOffset);
            emitSecRel(pp, f->symbolIndex, pp.symCOIndex, scope->backendStart);
            const auto endOffsetVar = scope->backendEnd == 0 ? f->endAddress : scope->backendEnd;
            concat.addU16(static_cast<uint16_t>(endOffsetVar - scope->backendStart)); // Range
            emitEndRecord(pp);
        }
    }

    void emitLocalConstants(ScbeCpu& pp, const Scope* scope, AstFuncDecl* const funcDecl)
    {
        for (const auto localConst : funcDecl->localConstants)
        {
            if (localConst->ownerScope != scope)
                continue;

            emitConstant(pp, localConst, localConst->token.text);
        }
    }

    bool emitScope(ScbeCpu& pp, CpuFunction* f, Scope* scope)
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
        emitSecRel(pp, f->symbolIndex, pp.symCOIndex, scope->backendStart);
        emitTruncatedString(pp, "");
        emitEndRecord(pp);

        const auto funcDecl = castAst<AstFuncDecl>(f->node);
        const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(funcDecl->typeInfo, TypeInfoKind::FuncAttr);

        // Local variables marked as global
        /////////////////////////////////
        emitLocalGlobalVars(pp, scope, concat, funcDecl);

        // Local constants
        /////////////////////////////////
        emitLocalConstants(pp, scope, funcDecl);

        // Local variables
        /////////////////////////////////
        emitLocalVars(pp, f, scope, typeFunc);

        // Sub scopes
        // Must be sorted, from first to last. We use the byte index of the first instruction in the block
        /////////////////////////////////
        if (scope->childrenScopes.size() > 1)
        {
            std::ranges::sort(scope->childrenScopes, [](const Scope* n1, const Scope* n2) { return n1->backendStart < n2->backendStart; });
        }

        for (const auto c : scope->childrenScopes)
            emitScope(pp, f, c);

        // End
        /////////////////////////////////
        emitStartRecord(pp, S_END);
        emitEndRecord(pp);
        return true;
    }

    void emitInlineTable(ScbeCpu& pp, Concat& concat, MapPath<uint32_t> mapFileNames, Vector<uint32_t> arrFileNames, Utf8 stringTable, const CpuFunction* f)
    {
        for (auto& dbgLines : f->dbgLines)
        {
            if (!dbgLines.inlined)
                continue;

            concat.addU32(DEBUG_S_INLINEELINES);
            const auto patchLTCount  = concat.addU32Addr(0); // Size of sub section
            const auto patchLTOffset = concat.totalCount();

            static constexpr uint32_t CV_INLINEE_SOURCE_LINE_SIGNATURE = 0;
            concat.addU32(CV_INLINEE_SOURCE_LINE_SIGNATURE);

            const auto checkSymIndex = getFileChecksum(mapFileNames, arrFileNames, stringTable, dbgLines.sourceFile);
            const auto typeIdx       = ScbeDebug::getOrCreateType(pp, dbgLines.inlined->typeInfo);

            for (const auto l : dbgLines.lines)
            {
                concat.addU32(typeIdx);
                concat.addU32(checkSymIndex);
                concat.addU32(l.line);
            }

            *patchLTCount = concat.totalCount() - patchLTOffset;
        }
    }

    void emitFuncParameters(ScbeCpu& pp, const CpuFunction* cpuFct)
    {
        if (!cpuFct->node->parameters)
            return;
        if (cpuFct->node->hasAttribute(ATTRIBUTE_MESSAGE_FUNC))
            return;

        auto&      concat      = pp.concat;
        const auto countParams = cpuFct->node->parameters->childCount();
        uint32_t   regCounter  = 0;
        for (uint32_t i = 0; i < countParams; i++)
        {
            const auto child     = cpuFct->node->parameters->children[i];
            const auto typeParam = cpuFct->typeFunc->parameters[i]->typeInfo;

            ScbeDebugTypeIndex typeIdx;
            switch (typeParam->kind)
            {
                case TypeInfoKind::Struct:
                    if (cpuFct->typeFunc->structParamByValue(typeParam))
                        typeIdx = ScbeDebug::getOrCreateType(pp, typeParam);
                    else
                        typeIdx = ScbeDebug::getOrCreatePointerToType(pp, typeParam, true);
                    break;

                case TypeInfoKind::Pointer:
                    if (typeParam->isAutoConstPointerRef())
                    {
                        const auto typeRef = TypeManager::concretePtrRefType(typeParam);
                        if (cpuFct->typeFunc->structParamByValue(typeRef))
                            typeIdx = ScbeDebug::getOrCreateType(pp, typeRef);
                        else
                            typeIdx = ScbeDebug::getOrCreateType(pp, typeParam);
                    }
                    else
                    {
                        typeIdx = ScbeDebug::getOrCreateType(pp, typeParam);
                    }
                    break;

                case TypeInfoKind::Array:
                    typeIdx = ScbeDebug::getOrCreatePointerToType(pp, typeParam, false);
                    break;

                default:
                    typeIdx = ScbeDebug::getOrCreateType(pp, typeParam);
                    break;
            }

            //////////
            uint32_t regParam = regCounter;
            if (cpuFct->typeFunc->isFctVariadic() && i != countParams - 1)
                regParam += 2;
            else if (cpuFct->typeFunc->isFctVariadic())
                regParam = 0;
            const uint32_t offsetStackParam = cpuFct->getStackOffsetParam(regParam);
            regCounter += typeParam->numRegisters();

            // If we have 2 registers then we cannot create a symbol flagged as 'fIsParam' to really see it.
            // But we also need to have one with the 'fIsParam' set to true, otherwise the callstack signature won't be correct.
            // So we create two: on dummy variable with 'fIsParam' set to true for the signature, and another one without the flag to be
            // able to see the value...
            if (typeParam->numRegisters() == 2)
            {
                //////////
                emitStartRecord(pp, S_LOCAL);
                concat.addU32(typeIdx);
                CV_LVARFLAGS cvf0    = {};
                cvf0.fIsParam        = true;
                cvf0.fIsOptimizedOut = true;
                cvf0.fCompGenx       = true;
                concat.addU16(std::bit_cast<uint16_t>(cvf0));
                emitTruncatedString(pp, "__" + child->token.text);
                emitEndRecord(pp);

                emitStartRecord(pp, S_DEFRANGE_REGISTER_REL);
                concat.addU16(R_RSP);
                concat.addU16(0); // Flags
                concat.addU32(offsetStackParam);
                emitSecRel(pp, cpuFct->symbolIndex, pp.symCOIndex);
                concat.addU16(static_cast<uint16_t>(cpuFct->endAddress - cpuFct->startAddress)); // Range
                emitEndRecord(pp);

                //////////
                emitStartRecord(pp, S_LOCAL);
                concat.addU32(typeIdx);
                CV_LVARFLAGS cvf1 = {};
                concat.addU16(std::bit_cast<uint16_t>(cvf1));
                emitTruncatedString(pp, child->token.text);
                emitEndRecord(pp);

                emitStartRecord(pp, S_DEFRANGE_REGISTER_REL);
                concat.addU16(R_RSP);
                concat.addU16(0); // Flags
                concat.addU32(offsetStackParam);
                emitSecRel(pp, cpuFct->symbolIndex, pp.symCOIndex);
                concat.addU16(static_cast<uint16_t>(cpuFct->endAddress - cpuFct->startAddress)); // Range
                emitEndRecord(pp);
            }
            else
            {
                //////////
                emitStartRecord(pp, S_LOCAL);
                concat.addU32(typeIdx);
                CV_LVARFLAGS cvf = {};
                cvf.fIsParam     = true; // set fIsParam. If not set, callstack signature won't be good.
                concat.addU16(std::bit_cast<uint16_t>(cvf));
                emitTruncatedString(pp, child->token.text);
                emitEndRecord(pp);

                emitStartRecord(pp, S_DEFRANGE_REGISTER_REL);
                concat.addU16(R_RSP);
                concat.addU16(0); // Flags
                concat.addU32(offsetStackParam);
                emitSecRel(pp, cpuFct->symbolIndex, pp.symCOIndex);
                concat.addU16(static_cast<uint16_t>(cpuFct->endAddress - cpuFct->startAddress)); // Range
                emitEndRecord(pp);
            }

            // codeview seems to need this pointer to be named "this"...
            // So add it
            if (cpuFct->typeFunc->isMethod() && child->token.is(g_LangSpec->name_self))
            {
                //////////
                emitStartRecord(pp, S_LOCAL);
                concat.addU32(typeIdx);
                CV_LVARFLAGS cvf = {};
                cvf.fIsParam     = true; // set fIsParam. If not set, callstack signature won't be good.
                concat.addU16(std::bit_cast<uint16_t>(cvf));
                emitTruncatedString(pp, "this");
                emitEndRecord(pp);

                //////////
                emitStartRecord(pp, S_DEFRANGE_REGISTER_REL);
                concat.addU16(R_RSP);
                concat.addU16(0); // Flags
                concat.addU32(offsetStackParam);
                emitSecRel(pp, cpuFct->symbolIndex, pp.symCOIndex);
                concat.addU16(static_cast<uint16_t>(cpuFct->endAddress - cpuFct->startAddress)); // Range
                emitEndRecord(pp);
            }
        }
    }

    void emitFuncCaptureParameters(ScbeCpu& pp, const CpuFunction* cpuFct)
    {
        if (!cpuFct->node->captureParameters)
            return;
        if (cpuFct->node->hasAttribute(ATTRIBUTE_MESSAGE_FUNC))
            return;

        auto&      concat      = pp.concat;
        const auto countParams = cpuFct->node->captureParameters->childCount();
        for (uint32_t i = 0; i < countParams; i++)
        {
            auto       child     = cpuFct->node->captureParameters->children[i];
            const auto typeParam = child->typeInfo;
            if (child->is(AstNodeKind::MakePointer))
                child = child->firstChild();
            const auto overload = child->resolvedSymbolOverload();
            if (!typeParam || !overload)
                continue;

            ScbeDebugTypeIndex typeIdx;
            switch (typeParam->kind)
            {
                case TypeInfoKind::Array:
                    typeIdx = ScbeDebug::getOrCreatePointerToType(pp, typeParam, false);
                    break;
                default:
                    typeIdx = ScbeDebug::getOrCreateType(pp, typeParam);
                    break;
            }

            //////////
            emitStartRecord(pp, S_LOCAL);
            concat.addU32(typeIdx); // Type
            CV_LVARFLAGS cvf = {};
            concat.addU16(std::bit_cast<uint16_t>(cvf));
            emitTruncatedString(pp, child->token.text);
            emitEndRecord(pp);

            emitStartRecord(pp, S_DEFRANGE_REGISTER_REL);
            concat.addU16(R_R12); // Register
            concat.addU16(0);     // Flags
            concat.addU32(overload->computedValue.storageOffset);
            emitSecRel(pp, cpuFct->symbolIndex, pp.symCOIndex);
            concat.addU16(static_cast<uint16_t>(cpuFct->endAddress - cpuFct->startAddress)); // Range
            emitEndRecord(pp);
        }
    }

    bool emitFctDebugS(ScbeCpu& pp)
    {
        auto& concat = pp.concat;

        MapPath<uint32_t> mapFileNames;
        Vector<uint32_t>  arrFileNames;
        Utf8              stringTable;

        for (const auto cpuFct : pp.functions)
        {
            if (!cpuFct->node || cpuFct->node->isSpecialFunctionGenerated())
                continue;

            // Add a func id type record
            /////////////////////////////////
            const auto tr = ScbeDebug::addTypeRecord(pp);
            tr->node      = cpuFct->node;
            if (cpuFct->typeFunc->isMethod())
            {
                tr->kind                 = LF_MFUNC_ID;
                const auto typeThis      = castTypeInfo<TypeInfoPointer>(cpuFct->typeFunc->parameters[0]->typeInfo, TypeInfoKind::Pointer);
                tr->lfMFuncId.parentType = ScbeDebug::getOrCreateType(pp, typeThis->pointedType);
                tr->lfMFuncId.type       = ScbeDebug::getOrCreateType(pp, cpuFct->typeFunc);
            }
            else
            {
                tr->kind          = LF_FUNC_ID;
                tr->lfFuncId.type = ScbeDebug::getOrCreateType(pp, cpuFct->typeFunc);
            }

            // Symbol
            /////////////////////////////////
            concat.addU32(DEBUG_S_SYMBOLS);
            const auto patchSCount  = concat.addU32Addr(0);
            const auto patchSOffset = concat.totalCount();

            // Proc ID
            /////////////////////////////////
            emitStartRecord(pp, S_LPROC32_ID);
            concat.addU32(0);                                         // Parent = 0
            concat.addU32(0);                                         // End = 0
            concat.addU32(0);                                         // Next = 0
            concat.addU32(cpuFct->endAddress - cpuFct->startAddress); // CodeSize = 0
            concat.addU32(0);                                         // DbgStart = 0
            concat.addU32(0);                                         // DbgEnd = 0
            concat.addU32(tr->index);                                 // FuncID type index
            emitSecRel(pp, cpuFct->symbolIndex, pp.symCOIndex);
            concat.addU8(0); // ProcSymFlags Flags = ProcSymFlags::None
            auto scopedName = ScbeDebug::getScopedName(cpuFct->node);
            emitTruncatedString(pp, scopedName);
            emitEndRecord(pp);

            // Frame Proc
            /////////////////////////////////
            emitStartRecord(pp, S_FRAMEPROC);
            concat.addU32(cpuFct->frameSize); // FrameSize
            concat.addU32(0);                 // Padding
            concat.addU32(0);                 // Offset of padding
            concat.addU32(0);                 // Bytes of callee saved registers
            concat.addU32(0);                 // Exception handler offset
            concat.addU32(0);                 // Exception handler section
            concat.addU32(0);                 // Flags (defines frame register)
            emitEndRecord(pp);

            // Capture parameters
            /////////////////////////////////
            emitFuncCaptureParameters(pp, cpuFct);

            // Parameters
            /////////////////////////////////
            emitFuncParameters(pp, cpuFct);

            // Lexical blocks
            /////////////////////////////////
            emitScope(pp, cpuFct, cpuFct->node->scope);

            // End
            /////////////////////////////////
            emitStartRecord(pp, S_PROC_ID_END);
            emitEndRecord(pp);

            *patchSCount = concat.totalCount() - patchSOffset;

            // inline lines table
            /////////////////////////////////
            emitInlineTable(pp, concat, mapFileNames, arrFileNames, stringTable, cpuFct);

            // Lines table
            /////////////////////////////////
            for (uint32_t idxDbgLines = 0; idxDbgLines < cpuFct->dbgLines.size(); idxDbgLines++)
                emitLines(pp, mapFileNames, arrFileNames, stringTable, concat, cpuFct, idxDbgLines);
        }

        // File checksum table
        /////////////////////////////////
        concat.addU32(DEBUG_S_FILECHKSMS);
        concat.addU32(arrFileNames.size() * 8); // Size of sub section
        for (const auto& p : arrFileNames)
        {
            concat.addU32(p); // Offset of file name in string table
            concat.addU32(0);
        }

        // String table
        /////////////////////////////////
        concat.addU32(DEBUG_S_STRINGTABLE);
        while (stringTable.length() & 3) // Align to 4 bytes
            stringTable.append(static_cast<char>(0));
        concat.addU32(stringTable.length());
        concat.addStringN(stringTable.data(), stringTable.length());

        return true;
    }
}

bool ScbeDebugCodeView::emit(const BuildParameters& buildParameters, ScbeCpu& pp)
{
    auto&      concat = pp.concat;
    const auto module = buildParameters.module;

    // .debug$S
    concat.align(16);
    *pp.patchDBGSOffset = concat.totalCount();
    concat.addU32(DEBUG_SECTION_MAGIC);
    if (buildParameters.buildCfg->backendDebugInfos)
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
    if (buildParameters.buildCfg->backendDebugInfos)
        emitDataDebugT(pp);
    *pp.patchDBGTCount = concat.totalCount() - *pp.patchDBGTOffset;

    // Reloc table .debug$S
    if (!pp.relocTableDBGSSection.table.empty())
    {
        concat.align(16);
        *pp.patchDBGSSectionRelocTableOffset = concat.totalCount();
        ScbeCoff::emitRelocationTable(pp.concat, pp.relocTableDBGSSection, pp.patchDBGSSectionFlags, pp.patchDBGSSectionRelocTableCount);
    }

    return true;
}
