#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVMDbg.h"
#include "ByteCode.h"
#include "SourceFile.h"
#include "Ast.h"
#include "TypeInfo.h"
#include "Module.h"
#include "BackendLLVM.h"
#include "SymTable.h"
#include "BuildParameters.h"
#include "Version.h"
#include "TypeManager.h"
#include "CommandLine.h"
#include "BackendLLVM.h"

void BackendLLVMDbg::setup(BackendLLVM* m, llvm::Module* modu)
{
    llvm             = m;
    isOptimized      = m->module->buildParameters.buildCfg->backendOptimizeSpeed || m->module->buildParameters.buildCfg->backendOptimizeSize;
    dbgBuilder       = new llvm::DIBuilder(*modu, true);
    llvmModule       = modu;
    llvmContext      = &modu->getContext();
    mainFile         = dbgBuilder->createFile("<stdin>", "c:/");
    fs::path expPath = m->bufferSwg.path;
    exportFile       = dbgBuilder->createFile(m->bufferSwg.name.c_str(), expPath.parent_path().string().c_str());
    Utf8 compiler    = format("swag %d.%d.%d", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM);
    compileUnit      = dbgBuilder->createCompileUnit(llvm::dwarf::DW_LANG_C99,
                                                mainFile,
                                                compiler.c_str(),
                                                isOptimized,
                                                "",
                                                0,
                                                "",
                                                llvm::DICompileUnit::DebugEmissionKind::FullDebug,
                                                0);

    modu->addModuleFlag(llvm::Module::Warning, "Debug Info Version", llvm::DEBUG_METADATA_VERSION);
#ifdef _WIN32
    modu->addModuleFlag(llvm::Module::Warning, "CodeView", 1);
#endif

    s8Ty    = dbgBuilder->createBasicType("s8", 8, llvm::dwarf::DW_ATE_signed);
    s16Ty   = dbgBuilder->createBasicType("s16", 16, llvm::dwarf::DW_ATE_signed);
    s32Ty   = dbgBuilder->createBasicType("s32", 32, llvm::dwarf::DW_ATE_signed);
    s64Ty   = dbgBuilder->createBasicType("s64", 64, llvm::dwarf::DW_ATE_signed);
    u8Ty    = dbgBuilder->createBasicType("u8", 8, llvm::dwarf::DW_ATE_unsigned);
    u16Ty   = dbgBuilder->createBasicType("u16", 16, llvm::dwarf::DW_ATE_unsigned);
    u32Ty   = dbgBuilder->createBasicType("u32", 32, llvm::dwarf::DW_ATE_unsigned);
    u64Ty   = dbgBuilder->createBasicType("u64", 64, llvm::dwarf::DW_ATE_unsigned);
    f32Ty   = dbgBuilder->createBasicType("f32", 32, llvm::dwarf::DW_ATE_float);
    f64Ty   = dbgBuilder->createBasicType("f64", 64, llvm::dwarf::DW_ATE_float);
    boolTy  = dbgBuilder->createBasicType("bool", 8, llvm::dwarf::DW_ATE_boolean);
    charTy  = dbgBuilder->createBasicType("char", 32, llvm::dwarf::DW_ATE_UTF);
    ptrU8Ty = dbgBuilder->createPointerType(u8Ty, 64);

    // string
    {
        auto              v1      = dbgBuilder->createMemberType(mainFile->getScope(), "data", mainFile, 0, 64, 0, 0, llvm::DINode::DIFlags::FlagZero, ptrU8Ty);
        auto              v2      = dbgBuilder->createMemberType(mainFile->getScope(), "sizeof", mainFile, 1, 32, 0, 64, llvm::DINode::DIFlags::FlagZero, u32Ty);
        llvm::DINodeArray content = dbgBuilder->getOrCreateArray({v1, v2});
        stringTy                  = dbgBuilder->createStructType(mainFile->getScope(), "string", mainFile, 0, 2 * sizeof(void*) * 8, 0, llvm::DINode::DIFlags::FlagZero, nullptr, content);
    }

    // interface
    {
        auto              v1      = dbgBuilder->createMemberType(mainFile->getScope(), "data", mainFile, 0, 64, 0, 0, llvm::DINode::DIFlags::FlagZero, ptrU8Ty);
        auto              v2      = dbgBuilder->createMemberType(mainFile->getScope(), "itable", mainFile, 1, 64, 0, 64, llvm::DINode::DIFlags::FlagZero, ptrU8Ty);
        llvm::DINodeArray content = dbgBuilder->getOrCreateArray({v1, v2});
        interfaceTy               = dbgBuilder->createStructType(mainFile->getScope(), "interface", mainFile, 0, 2 * sizeof(void*) * 8, 0, llvm::DINode::DIFlags::FlagZero, nullptr, content);
    }
}

llvm::DIFile* BackendLLVMDbg::getOrCreateFile(SourceFile* file)
{
    auto it = mapFiles.find(file->path);
    if (it != mapFiles.end())
        return it->second;

    fs::path      filePath = file->path;
    llvm::DIFile* dbgfile  = dbgBuilder->createFile(filePath.filename().string().c_str(), filePath.parent_path().string().c_str());
    mapFiles[file->path]   = dbgfile;
    return dbgfile;
}

llvm::DIType* BackendLLVMDbg::getEnumType(TypeInfo* typeInfo, llvm::DIFile* file)
{
    auto it = mapTypes.find(typeInfo);
    if (it != mapTypes.end())
        return it->second;

    // Normal c enum, integer only
    auto typeInfoEnum = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
    if (typeInfoEnum->rawType->flags & TYPEINFO_INTEGER)
    {
        auto                          scope = file->getScope();
        VectorNative<llvm::Metadata*> subscripts;
        bool                          isUnsigned = typeInfoEnum->rawType->flags & TYPEINFO_UNSIGNED;
        for (auto& value : typeInfoEnum->values)
        {
            auto typeField = dbgBuilder->createEnumerator(value->namedParam.c_str(), value->value.reg.u64, isUnsigned);
            subscripts.push_back(typeField);
        }

        auto lineNo        = typeInfo->declNode->token.startLocation.line + 1;
        auto rawType       = getType(typeInfoEnum->rawType, file);
        auto content       = dbgBuilder->getOrCreateArray({subscripts.begin(), subscripts.end()});
        auto result        = dbgBuilder->createEnumerationType(scope, typeInfo->name.c_str(), file, lineNo, typeInfo->sizeOf * 8, 0, content, rawType);
        mapTypes[typeInfo] = result;
        return result;
    }

    return getType(typeInfoEnum->rawType, file);
}

llvm::DIType* BackendLLVMDbg::getPointerToType(TypeInfo* typeInfo, llvm::DIFile* file)
{
    auto it = mapPtrTypes.find(typeInfo);
    if (it != mapPtrTypes.end())
        return it->second;
    auto result           = dbgBuilder->createPointerType(getType(typeInfo, file), 64);
    mapPtrTypes[typeInfo] = result;
    return result;
}

llvm::DIType* BackendLLVMDbg::getReferenceToType(TypeInfo* typeInfo, llvm::DIFile* file)
{
    auto it = mapRefTypes.find(typeInfo);
    if (it != mapRefTypes.end())
        return it->second;
    auto result           = dbgBuilder->createReferenceType(llvm::dwarf::DW_TAG_reference_type, getType(typeInfo, file));
    mapRefTypes[typeInfo] = result;
    return result;
}

llvm::DIType* BackendLLVMDbg::getStructType(TypeInfo* typeInfo, llvm::DIFile* file)
{
    auto it = mapTypes.find(typeInfo);
    if (it != mapTypes.end())
        return it->second;

    auto scope  = file->getScope();
    auto noFlag = llvm::DINode::DIFlags::FlagZero;
    auto lineNo = typeInfo->declNode->token.startLocation.line + 1;
    auto name   = typeInfo->name;
    Ast::normalizeIdentifierName(name);
    auto result = dbgBuilder->createStructType(scope, name.c_str(), file, lineNo, typeInfo->sizeOf * 8, 0, noFlag, nullptr, llvm::DINodeArray());

    // Register it right away, so that even if a struct is referencing itself, this will work
    // and won't recurse forever
    mapTypes[typeInfo] = result;

    // Deal with the struct content now that the struct is registered
    VectorNative<llvm::Metadata*> subscripts;
    auto                          typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    for (auto& field : typeStruct->fields)
    {
        auto fieldLine = field->node->token.startLocation.line + 1;
        auto fieldType = getType(field->typeInfo, file);
        auto typeField = dbgBuilder->createMemberType(result, field->namedParam.c_str(), file, fieldLine, field->sizeOf * 8, 0, field->offset * 8, noFlag, fieldType);
        subscripts.push_back(typeField);
    }

    auto content = dbgBuilder->getOrCreateArray({subscripts.begin(), subscripts.end()});
    dbgBuilder->replaceArrays(result, content);

    return result;
}

llvm::DIType* BackendLLVMDbg::getSliceType(TypeInfo* typeInfo, llvm::DIFile* file)
{
    auto it = mapTypes.find(typeInfo);
    if (it != mapTypes.end())
        return it->second;

    auto typeInfoPtr = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
    auto fileScope   = file->getScope();
    auto noFlag      = llvm::DINode::DIFlags::FlagZero;
    auto name        = "__autoslice_" + typeInfoPtr->name;
    Ast::normalizeIdentifierName(name);
    auto result        = dbgBuilder->createStructType(fileScope, name.c_str(), file, 0, 2 * sizeof(void*) * 8, 0, noFlag, nullptr, llvm::DINodeArray());
    mapTypes[typeInfo] = result;

    auto realType = getPointerToType(typeInfoPtr->pointedType, file);
    auto v1       = dbgBuilder->createMemberType(result, "data", file, 0, 64, 0, 0, noFlag, realType);
    auto v2       = dbgBuilder->createMemberType(result, "count", file, 1, 32, 0, 64, noFlag, u32Ty);
    auto content  = dbgBuilder->getOrCreateArray({v1, v2});
    dbgBuilder->replaceArrays(result, content);

    return result;
}

llvm::DIType* BackendLLVMDbg::getType(TypeInfo* typeInfo, llvm::DIFile* file)
{
    if (!typeInfo)
        return s32Ty;

    auto it = mapTypes.find(typeInfo);
    if (it != mapTypes.end())
        return it->second;

    switch (typeInfo->kind)
    {
    case TypeInfoKind::Alias:
    {
        auto typeInfoAlias = CastTypeInfo<TypeInfoAlias>(typeInfo, TypeInfoKind::Alias);
        auto scope         = getOrCreateScope(file, typeInfoAlias->declNode->ownerScope);
        auto retType       = dbgBuilder->createTypedef(getType(typeInfoAlias->rawType, file), typeInfoAlias->name.c_str(), file, typeInfoAlias->declNode->token.startLocation.line + 1, scope);
        mapTypes[typeInfo] = retType;
        return retType;
    }

    case TypeInfoKind::Enum:
        return getEnumType(typeInfo, file);
    case TypeInfoKind::Struct:
        return getStructType(typeInfo, file);

    case TypeInfoKind::Array:
    {
        auto                          typeInfoPtr = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        VectorNative<llvm::Metadata*> subscripts;

        auto* countNode = llvm::ConstantAsMetadata::get(llvm::ConstantInt::getSigned(llvm::Type::getInt64Ty(*llvmContext), typeInfoPtr->count));
        subscripts.push_back(dbgBuilder->getOrCreateSubrange(countNode, nullptr, nullptr, nullptr));

        auto subscriptArray = dbgBuilder->getOrCreateArray({subscripts.begin(), subscripts.end()});
        auto result         = dbgBuilder->createArrayType(typeInfoPtr->totalCount, 0, getType(typeInfoPtr->pointedType, file), subscriptArray);
        mapTypes[typeInfo]  = result;
        return result;
    }
    case TypeInfoKind::Pointer:
    {
        auto typeInfoPtr   = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        auto result        = getPointerToType(typeInfoPtr->pointedType, file);
        mapTypes[typeInfo] = result;
        return result;
    }
    case TypeInfoKind::Reference:
    {
        auto typeInfoPtr   = CastTypeInfo<TypeInfoReference>(typeInfo, TypeInfoKind::Reference);
        auto result        = getPointerToType(typeInfoPtr->pointedType, file);
        mapTypes[typeInfo] = result;
        return result;
    }
    case TypeInfoKind::Slice:
        return getSliceType(typeInfo, file);

    case TypeInfoKind::Interface:
        return interfaceTy;

    case TypeInfoKind::Native:
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
            return s8Ty;
        case NativeTypeKind::S16:
            return s16Ty;
        case NativeTypeKind::S32:
            return s32Ty;
        case NativeTypeKind::S64:
            return s64Ty;
        case NativeTypeKind::U8:
            return u8Ty;
        case NativeTypeKind::U16:
            return u16Ty;
        case NativeTypeKind::U32:
            return u32Ty;
        case NativeTypeKind::U64:
            return u64Ty;
        case NativeTypeKind::F32:
            return f32Ty;
        case NativeTypeKind::F64:
            return f64Ty;
        case NativeTypeKind::Char:
            return charTy;
        case NativeTypeKind::Bool:
            return boolTy;
        case NativeTypeKind::String:
            return stringTy;
        default:
            return s64Ty;
        }
    }
    }

    return s64Ty;
}

llvm::DISubroutineType* BackendLLVMDbg::getFunctionType(TypeInfoFuncAttr* typeFunc, llvm::DIFile* file)
{
    auto it = mapFuncTypes.find(typeFunc);
    if (it != mapFuncTypes.end())
        return it->second;

    VectorNative<llvm::Metadata*> params;

    if (typeFunc->returnType)
    {
        for (int r = 0; r < typeFunc->returnType->numRegisters(); r++)
            params.push_back(getType(g_TypeMgr.typeInfoPVoid, file));
    }

    for (auto one : typeFunc->parameters)
    {
        for (int r = 0; r < one->typeInfo->numRegisters(); r++)
            params.push_back(getType(g_TypeMgr.typeInfoPVoid, file));
    }

    auto typeArray = dbgBuilder->getOrCreateTypeArray({params.begin(), params.end()});
    auto result    = dbgBuilder->createSubroutineType(typeArray);

    mapFuncTypes[typeFunc] = result;
    return result;
}

void BackendLLVMDbg::startFunction(LLVMPerThread& pp, ByteCode* bc, llvm::Function* func, llvm::AllocaInst* stack)
{
    SWAG_ASSERT(dbgBuilder);

    Utf8         name = bc->name;
    AstFuncDecl* decl = nullptr;

    int               line     = 0;
    TypeInfoFuncAttr* typeFunc = bc->typeInfoFunc;
    if (bc->node)
    {
        decl     = CastAst<AstFuncDecl>(bc->node, AstNodeKind::FuncDecl);
        name     = decl->name;
        line     = decl->token.startLocation.line;
        typeFunc = CastTypeInfo<TypeInfoFuncAttr>(decl->typeInfo, TypeInfoKind::FuncAttr);
    }

    // Type
    llvm::DIFile*           file        = getOrCreateFile(bc->sourceFile);
    unsigned                lineNo      = line + 1;
    llvm::DISubroutineType* dbgFuncType = getFunctionType(typeFunc, file);

    // Flags
    llvm::DISubprogram::DISPFlags spFlags = llvm::DISubprogram::SPFlagDefinition;
    if (isOptimized)
        spFlags |= llvm::DISubprogram::SPFlagOptimized;
    if (!decl || !(decl->attributeFlags & ATTRIBUTE_PUBLIC))
        spFlags |= llvm::DISubprogram::SPFlagLocalToUnit;

    llvm::DINode::DIFlags diFlags = llvm::DINode::FlagPrototyped | llvm::DINode::FlagStaticMember;

    // Register function
    llvm::DISubprogram* SP = dbgBuilder->createFunction(file, name.c_str(), llvm::StringRef(), file, lineNo, dbgFuncType, lineNo, diFlags, spFlags);
    func->setSubprogram(SP);
    if (decl)
        mapScopes[decl->content->ownerScope] = SP;

    // Parameters
    if (decl && decl->parameters && !(decl->attributeFlags & ATTRIBUTE_COMPILER_FUNC))
    {
        auto idxParam    = typeFunc->numReturnRegisters();
        auto countParams = decl->parameters->childs.size();
        if (typeFunc->flags & TYPEINFO_VARIADIC)
        {
            idxParam += 2;
            countParams--;
        }

        for (int i = 0; i < countParams; i++)
        {
            auto        child     = decl->parameters->childs[i];
            const auto& loc       = child->token.startLocation;
            auto        typeParam = typeFunc->parameters[i]->typeInfo;
            auto        location  = llvm::DebugLoc::get(loc.line + 1, loc.column, SP);

            llvm::DINode::DIFlags flags = llvm::DINode::FlagZero;
            if (typeParam->flags & TYPEINFO_SELF)
                flags |= llvm::DINode::FlagObjectPointer;

            llvm::DIType* type = nullptr;
            switch (typeParam->kind)
            {
            case TypeInfoKind::Array:
                type = getPointerToType(typeParam, file);
                break;

            default:
                type = getType(typeParam, file);
                break;
            }

            llvm::DILocalVariable* var = dbgBuilder->createParameterVariable(SP, child->name.c_str(), i + 1, file, loc.line + 1, type, !isOptimized, flags);
            dbgBuilder->insertDeclare(func->getArg(idxParam), var, dbgBuilder->createExpression(), location, pp.builder->GetInsertBlock());
            idxParam += typeParam->numRegisters();
        }
    }

    // Local variables
    for (auto localVar : bc->localVars)
    {
        SymbolOverload* overload = localVar->resolvedSymbolOverload;
        auto            typeInfo = overload->typeInfo;

        llvm::DIType*          type  = getType(typeInfo, file);
        auto                   scope = getOrCreateScope(file, localVar->ownerScope);
        llvm::DILocalVariable* var   = dbgBuilder->createAutoVariable(scope, localVar->name.c_str(), file, localVar->token.startLocation.line, type, !isOptimized);

        auto& loc = localVar->token.startLocation;
        auto  v   = pp.builder->CreateInBoundsGEP(stack, pp.builder->getInt32(overload->storageOffset));
        dbgBuilder->insertDeclare(v, var, dbgBuilder->createExpression(), llvm::DebugLoc::get(loc.line + 1, loc.column, scope), pp.builder->GetInsertBlock());
    }
}

void BackendLLVMDbg::startWrapperFunction(LLVMPerThread& pp, ByteCode* bc, AstFuncDecl* node, llvm::Function* func)
{
    auto                    file        = exportFile;
    Utf8                    name        = node->fullnameForeign;
    auto                    lineNo      = node->exportForeignLine;
    llvm::DISubroutineType* dbgFuncType = getFunctionType(bc->callType(), file);

    // Flags
    llvm::DISubprogram::DISPFlags spFlags = llvm::DISubprogram::SPFlagDefinition;
    if (isOptimized)
        spFlags |= llvm::DISubprogram::SPFlagOptimized;

    llvm::DINode::DIFlags diFlags = llvm::DINode::FlagPrototyped | llvm::DINode::FlagStaticMember;

    // Register function
    llvm::DISubprogram* SP = dbgBuilder->createFunction(file, name.c_str(), llvm::StringRef(), file, lineNo, dbgFuncType, lineNo, diFlags, spFlags);
    func->setSubprogram(SP);

    pp.builder->SetCurrentDebugLocation(llvm::DebugLoc::get(lineNo, 0, SP));
}

void BackendLLVMDbg::setLocation(llvm::IRBuilder<>* builder, ByteCode* bc, ByteCodeInstruction* ip)
{
    SWAG_ASSERT(dbgBuilder);
    if (!ip || !ip->node || !ip->node->ownerScope || ip->node->kind == AstNodeKind::FuncDecl || (ip->flags & BCI_SAFETY))
    {
        builder->SetCurrentDebugLocation(llvm::DebugLoc());
    }
    else
    {
        auto location = ip->getLocation(bc);
        if (location)
        {
            llvm::DIFile*  file  = getOrCreateFile(ip->getFileLocation(bc));
            llvm::DIScope* scope = getOrCreateScope(file, ip->node->ownerScope);
            builder->SetCurrentDebugLocation(llvm::DebugLoc::get(location->line + 1, 0, scope));
        }
    }
}

llvm::DIScope* BackendLLVMDbg::getOrCreateScope(llvm::DIFile* file, Scope* scope)
{
    SWAG_ASSERT(scope);

    llvm::DIScope*       parent = file;
    VectorNative<Scope*> toGen;
    auto                 scanScope = scope;
    while (scanScope->kind != ScopeKind::Module && scanScope->kind != ScopeKind::File)
    {
        auto it = mapScopes.find(scanScope);
        if (it != mapScopes.end())
        {
            parent = it->second;
            break;
        }

        toGen.push_back(scanScope);
        scanScope = scanScope->parentScope;
    }

    if (toGen.empty())
        return parent;

    for (int i = (int) toGen.size() - 1; i >= 0; i--)
    {
        auto           toGenScope = toGen[i];
        llvm::DIScope* newScope;
        if (toGenScope->isGlobal())
        {
            newScope = dbgBuilder->createLexicalBlockFile(parent, file);
        }
        else
        {
            auto lineNo = toGenScope->owner ? toGenScope->owner->token.startLocation.line + 1 : 0;
            newScope    = dbgBuilder->createLexicalBlock(parent, file, lineNo, 0);
        }
        mapScopes[toGenScope] = newScope;
        parent                = newScope;
    }

    return parent;
}

void BackendLLVMDbg::createGlobalVariablesForSegment(const BuildParameters& buildParameters, llvm::Type* type, llvm::GlobalVariable* var)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = llvm->perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;
    auto& modu            = *pp.module;
    auto  module          = llvm->module;

    VectorNative<llvm::Value*> offset;
    offset.reserve(2);
    offset.push_back(builder.getInt32(0));
    offset.push_back(builder.getInt32(0));

    auto& all = var == pp.bssSeg ? module->globalVarsBss : module->globalVarsMutable;
    for (auto node : all)
    {
        auto resolved = node->resolvedSymbolOverload;
        auto typeInfo = node->typeInfo;
        auto file     = compileUnit->getFile();
        auto dbgType  = getType(typeInfo, file);
        if (!dbgType)
            continue;

        // Segment is stored as an array of 64 bits elements. Get address of the first element
        auto constExpr = llvm::ConstantExpr::getGetElementPtr(type, var, {offset.begin(), offset.end()});
        // Convert to a pointer to bytes
        constExpr = llvm::ConstantExpr::getPointerCast(constExpr, builder.getInt8Ty()->getPointerTo());
        // Convert to int, in order to make an Add
        constExpr = llvm::ConstantExpr::getPtrToInt(constExpr, builder.getInt64Ty());
        // Add the byte offset
        constExpr = llvm::ConstantExpr::getAdd(constExpr, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*pp.context), resolved->storageOffset));
        // Convert back to a pointer to bytes
        constExpr = llvm::ConstantExpr::getIntToPtr(constExpr, builder.getInt8Ty()->getPointerTo());

        // Cast to the correct type
        llvm::Type* varType = nullptr;
        if (!llvm->swagTypeToLLVMType(buildParameters, module, typeInfo, &varType))
            continue;

        varType         = varType->getPointerTo();
        constExpr       = llvm::ConstantExpr::getPointerCast(constExpr, varType);
        auto name       = node->name.c_str();
        auto g          = new llvm::GlobalVariable(modu, varType, false, llvm::GlobalValue::ExternalLinkage, constExpr, name);
        auto dbgRefType = getReferenceToType(typeInfo, file);
        auto gve        = pp.dbg->dbgBuilder->createGlobalVariableExpression(compileUnit, name, name, file, 0, dbgRefType, dbgType, true);
        g->addDebugInfo(gve);
    }
}

void BackendLLVMDbg::finalize()
{
    SWAG_ASSERT(dbgBuilder);
    dbgBuilder->finalize();
}
