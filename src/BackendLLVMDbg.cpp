#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVMDbg.h"
#include "ByteCode.h"
#include "Module.h"
#include "Version.h"
#include "TypeManager.h"
#include "Workspace.h"

static llvm::DILocation* debugLocGet(unsigned Line, unsigned Col, const llvm::MDNode* Scope, const llvm::MDNode* InlinedAt = nullptr, bool ImplicitCode = false)
{
    return llvm::DILocation::get(Scope->getContext(), Line, Col, const_cast<llvm::MDNode*>(Scope), const_cast<llvm::MDNode*>(InlinedAt), ImplicitCode);
}

void BackendLLVMDbg::setup(BackendLLVM* m, llvm::Module* modu)
{
    llvm          = m;
    isOptimized   = m->module->buildParameters.buildCfg->backendOptimizeSpeed || m->module->buildParameters.buildCfg->backendOptimizeSize;
    dbgBuilder    = new llvm::DIBuilder(*modu, true);
    llvmModule    = modu;
    llvmContext   = &modu->getContext();
    mainFile      = dbgBuilder->createFile("<stdin>", "c:/");
    Path expPath  = m->exportFilePath;
    exportFile    = dbgBuilder->createFile(m->exportFileName.string(), expPath.parent_path().string());
    Utf8 compiler = Fmt("swag %d.%d.%d", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM);
    compileUnit   = dbgBuilder->createCompileUnit(llvm::dwarf::DW_LANG_C99,
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
    charTy  = dbgBuilder->createBasicType("rune", 32, llvm::dwarf::DW_ATE_UTF);
    ptrU8Ty = dbgBuilder->createPointerType(u8Ty, 64);

    // string
    {
        auto              v1      = dbgBuilder->createMemberType(mainFile->getScope(), "data", mainFile, 0, 64, 0, 0, llvm::DINode::DIFlags::FlagZero, ptrU8Ty);
        auto              v2      = dbgBuilder->createMemberType(mainFile->getScope(), "sizeof", mainFile, 1, 64, 0, 64, llvm::DINode::DIFlags::FlagZero, u64Ty);
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

    // any
    {
        auto              ptrType = getPointerToType(g_Workspace->swagScope.regTypeInfo, mainFile);
        auto              v1      = dbgBuilder->createMemberType(mainFile->getScope(), "ptrvalue", mainFile, 0, 64, 0, 0, llvm::DINode::DIFlags::FlagZero, ptrU8Ty);
        auto              v2      = dbgBuilder->createMemberType(mainFile->getScope(), "typeinfo", mainFile, 1, 64, 0, 64, llvm::DINode::DIFlags::FlagZero, ptrType);
        llvm::DINodeArray content = dbgBuilder->getOrCreateArray({v1, v2});
        anyTy                     = dbgBuilder->createStructType(mainFile->getScope(), "any", mainFile, 0, 2 * sizeof(void*) * 8, 0, llvm::DINode::DIFlags::FlagZero, nullptr, content);
    }
}

llvm::DIFile* BackendLLVMDbg::getOrCreateFile(SourceFile* file)
{
    auto it = mapFiles.find(file->path);
    if (it != mapFiles.end())
        return it->second;

    Path          filePath = file->path;
    llvm::DIFile* dbgfile  = dbgBuilder->createFile(filePath.filename().string(), filePath.parent_path().string());
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
    if (typeInfoEnum->rawType->isNativeInteger())
    {
        auto                          scope = file->getScope();
        VectorNative<llvm::Metadata*> subscripts;
        bool                          isUnsigned = typeInfoEnum->rawType->flags & TYPEINFO_UNSIGNED;
        for (auto& value : typeInfoEnum->values)
        {
            auto typeField = dbgBuilder->createEnumerator(value->name.c_str(), value->value->reg.u64, isUnsigned);
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
    typeInfo->computeScopedName();
    auto result = dbgBuilder->createStructType(scope, name.c_str(), file, lineNo, typeInfo->sizeOf * 8, 0, noFlag, nullptr, llvm::DINodeArray(), 0, nullptr, typeInfo->scopedName.c_str());

    // Register it right away, so that even if a struct is referencing itself, this will work
    // and won't recurse forever
    mapTypes[typeInfo] = result;

    // Deal with the struct content now that the struct is registered
    VectorNative<llvm::Metadata*> subscripts;
    auto                          typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    for (auto& field : typeStruct->fields)
    {
        auto fieldLine = field->declNode->token.startLocation.line + 1;
        auto fieldType = getType(field->typeInfo, file);
        auto typeField = dbgBuilder->createMemberType(result, field->name.c_str(), file, fieldLine, field->typeInfo->sizeOf * 8, 0, field->offset * 8, noFlag, fieldType);
        subscripts.push_back(typeField);
    }

    llvm::ArrayRef<llvm::Metadata*> subscripts1{subscripts.begin(), subscripts.end()};
    auto                            content = dbgBuilder->getOrCreateArray(subscripts1);
    dbgBuilder->replaceArrays(result, content);

    return result;
}

llvm::DIType* BackendLLVMDbg::getSliceType(TypeInfo* typeInfo, TypeInfo* pointedType, llvm::DIFile* file)
{
    auto it = mapTypes.find(typeInfo);
    if (it != mapTypes.end())
        return it->second;

    auto fileScope = file->getScope();
    auto noFlag    = llvm::DINode::DIFlags::FlagZero;
    auto name      = Fmt("[..] %s", pointedType->name.c_str()); // debugger doesn't like 'const' before slice name
    auto result    = dbgBuilder->createStructType(fileScope, name.c_str(), file, 0, 2 * sizeof(void*) * 8, 0, noFlag, nullptr, llvm::DINodeArray());

    auto realType = getPointerToType(pointedType, file);
    auto v1       = dbgBuilder->createMemberType(result, "data", file, 0, 64, 0, 0, noFlag, realType);
    auto v2       = dbgBuilder->createMemberType(result, "count", file, 1, 64, 0, 64, noFlag, u64Ty);
    auto content  = dbgBuilder->getOrCreateArray({v1, v2});
    dbgBuilder->replaceArrays(result, content);

    mapTypes[typeInfo] = result;
    return result;
}

llvm::DIType* BackendLLVMDbg::getType(TypeInfo* typeInfo, llvm::DIFile* file)
{
    if (!typeInfo)
        return s32Ty;
    typeInfo = TypeManager::concreteType(typeInfo, CONCRETE_ALIAS);

    auto it = mapTypes.find(typeInfo);
    if (it != mapTypes.end())
        return it->second;

    switch (typeInfo->kind)
    {
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
        auto          typeInfoPtr = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        llvm::DIType* result      = nullptr;
        if (typeInfoPtr->isPointerArithmetic())
            result = getPointerToType(typeInfoPtr->pointedType, file);
        else
            result = getReferenceToType(typeInfoPtr->pointedType, file);
        mapTypes[typeInfo] = result;
        return result;
    }
    case TypeInfoKind::Slice:
    {
        auto typeInfoPtr = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        return getSliceType(typeInfo, typeInfoPtr->pointedType, file);
    }
    case TypeInfoKind::TypedVariadic:
    {
        auto typeInfoPtr = CastTypeInfo<TypeInfoVariadic>(typeInfo, TypeInfoKind::TypedVariadic);
        return getSliceType(typeInfo, typeInfoPtr->rawType, file);
    }
    case TypeInfoKind::Variadic:
    {
        return getSliceType(typeInfo, g_TypeMgr->typeInfoAny, file);
    }

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
        case NativeTypeKind::Rune:
            return charTy;
        case NativeTypeKind::Bool:
            return boolTy;
        case NativeTypeKind::String:
            return stringTy;
        case NativeTypeKind::Any:
            return anyTy;
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

    params.push_back(nullptr); // void

    if (typeFunc->returnType)
    {
        params.push_back(getType(typeFunc->returnType, file));
    }

    for (auto one : typeFunc->parameters)
    {
        auto typeInfo = TypeManager::concreteType(one->typeInfo);
        if (typeInfo->numRegisters() == 1)
        {
            params.push_back(getType(typeInfo, file));
        }
        else
        {
            for (int r = 0; r < one->typeInfo->numRegisters(); r++)
            {
                params.push_back(getType(g_TypeMgr->typeInfoU64, file));
            }
        }
    }

    auto typeArray = dbgBuilder->getOrCreateTypeArray({params.begin(), params.end()});
    auto result    = dbgBuilder->createSubroutineType(typeArray);

    mapFuncTypes[typeFunc] = result;
    return result;
}

llvm::DISubprogram* BackendLLVMDbg::startFunction(ByteCode* bc, AstFuncDecl** resultDecl)
{
    SWAG_ASSERT(dbgBuilder);

    Utf8              name     = bc->name;
    AstFuncDecl*      decl     = nullptr;
    int               line     = 0;
    TypeInfoFuncAttr* typeFunc = bc->typeInfoFunc;

    if (bc->node)
    {
        decl     = CastAst<AstFuncDecl>(bc->node, AstNodeKind::FuncDecl);
        name     = decl->token.text;
        line     = decl->token.startLocation.line;
        typeFunc = CastTypeInfo<TypeInfoFuncAttr>(decl->typeInfo, TypeInfoKind::FuncAttr);
    }

    if (resultDecl)
        *resultDecl = decl;

    // Already created ?
    auto it = mapScopes.find(decl->content->ownerScope);
    if (it != mapScopes.end())
        return (llvm::DISubprogram*) it->second;

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
    auto                mangledName = typeFunc->declNode->getScopedName();
    llvm::DISubprogram* SP          = dbgBuilder->createFunction(file, name.c_str(), mangledName.c_str(), file, lineNo, dbgFuncType, lineNo, diFlags, spFlags);
    if (decl)
        mapScopes[decl->content->ownerScope] = SP;

    return SP;
}

void BackendLLVMDbg::startFunction(const BuildParameters& buildParameters, LLVMPerThread& pp, ByteCode* bc, llvm::Function* func, llvm::AllocaInst* stack)
{
    if (bc->node && bc->node->isSpecialFunctionGenerated())
        return;

    auto& builder = *pp.builder;
    builder.SetCurrentDebugLocation(llvm::DebugLoc());
    lastDebugLine        = 0;
    lastInlineSourceFile = nullptr;

    AstFuncDecl*        decl;
    llvm::DISubprogram* SP = startFunction(bc, &decl);
    func->setSubprogram(SP);

    Utf8              name     = bc->name;
    TypeInfoFuncAttr* typeFunc = bc->typeInfoFunc;
    int               line     = 0;
    llvm::DIFile*     file     = getOrCreateFile(bc->sourceFile);
    if (decl)
    {
        name     = decl->token.text;
        line     = decl->token.startLocation.line;
        typeFunc = CastTypeInfo<TypeInfoFuncAttr>(decl->typeInfo, TypeInfoKind::FuncAttr);
    }

    // Parameters
    if (decl && decl->parameters && !(decl->attributeFlags & ATTRIBUTE_COMPILER_FUNC))
    {
        int  idxParam    = 0;
        auto countParams = decl->parameters->childs.size();

        // Variadic. Pass as first parameters, but get type at the end
        if (typeFunc->flags & (TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
        {
            auto          child     = decl->parameters->childs.back();
            const auto&   loc       = child->token.startLocation;
            auto          typeParam = typeFunc->parameters.back()->typeInfo;
            auto          scope     = SP;
            llvm::DIType* type      = getType(typeParam, file);

            llvm::DILocalVariable* var0   = dbgBuilder->createAutoVariable(scope, child->token.ctext(), file, loc.line + 1, type, !isOptimized);
            auto                   allocA = builder.CreateAlloca(builder.getInt64Ty(), builder.getInt64(2));

            auto v0   = builder.CreateInBoundsGEP(allocA, builder.getInt64(0));
            auto arg0 = func->getArg(0);
            builder.CreateStore(arg0, builder.CreatePointerCast(v0, arg0->getType()->getPointerTo()));

            auto v1 = builder.CreateInBoundsGEP(allocA, builder.getInt64(1));
            builder.CreateStore(func->getArg(1), v1);

            dbgBuilder->insertDeclare(allocA, var0, dbgBuilder->createExpression(), debugLocGet(loc.line + 1, loc.column, scope), pp.builder->GetInsertBlock());

            idxParam += 2;
            countParams--;
        }
        else if (typeFunc->flags & TYPEINFO_C_VARIADIC)
        {
            countParams--;
        }

        for (int i = 0; i < countParams; i++)
        {
            auto        child     = decl->parameters->childs[i];
            const auto& loc       = child->token.startLocation;
            auto        typeParam = typeFunc->parameters[i]->typeInfo;
            auto        scope     = SP;
            auto        location  = debugLocGet(loc.line + 1, loc.column, scope);

            // If parameters are passed with two registers, make a local variable instead of a reference to
            // the parameters
            if (typeParam->isString() || typeParam->isSlice())
            {
                auto type = getType(typeParam, file);

                llvm::DILocalVariable* var0   = dbgBuilder->createAutoVariable(scope, child->token.ctext(), file, loc.line + 1, type, !isOptimized);
                auto                   allocA = builder.CreateAlloca(builder.getInt64Ty(), builder.getInt64(2));

                auto v0   = builder.CreateInBoundsGEP(allocA, builder.getInt64(0));
                auto arg0 = func->getArg(idxParam);
                builder.CreateStore(arg0, builder.CreatePointerCast(v0, arg0->getType()->getPointerTo()));

                auto v1 = builder.CreateInBoundsGEP(allocA, builder.getInt64(1));
                builder.CreateStore(func->getArg(idxParam + 1), v1);

                dbgBuilder->insertDeclare(allocA, var0, dbgBuilder->createExpression(), debugLocGet(loc.line + 1, loc.column, scope), pp.builder->GetInsertBlock());
            }
            else
            {
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

                llvm::DILocalVariable* var   = dbgBuilder->createParameterVariable(scope, child->token.ctext(), idxParam + 1, file, loc.line + 1, type, !isOptimized, llvm::DINode::FlagZero);
                llvm::Value*           value = func->getArg(idxParam);

                // :OptimizedAwayDebugCrap
                // Parameters are "optimized away" by the debugger, most of the time.
                // The only way to have correct values seems to make a local copy of the parameter on the stack,
                // and make the debugger use that copy instead of the parameter.
                // Crap !
                // So make a copy, except if we are in optimized mode...
                bool isDebug = !buildParameters.buildCfg->backendOptimizeSpeed && !buildParameters.buildCfg->backendOptimizeSize;
                if (isDebug)
                {
                    auto allocA = builder.CreateAlloca(value->getType(), nullptr, func->getArg(idxParam)->getName());
                    builder.CreateStore(value, allocA);
                    value = allocA;
                }

                dbgBuilder->insertDeclare(value, var, dbgBuilder->createExpression(), location, pp.builder->GetInsertBlock());

                if (typeParam->isSelf() && child->token.text == "self")
                {
                    var = dbgBuilder->createParameterVariable(scope, "this", idxParam + 1, file, loc.line + 1, type, !isOptimized, llvm::DINode::FlagObjectPointer);
                    dbgBuilder->insertDeclare(value, var, dbgBuilder->createExpression(), location, pp.builder->GetInsertBlock());
                }
            }

            idxParam += typeParam->numRegisters();
        }
    }

    // Local variables
    for (auto localVar : bc->localVars)
    {
        SymbolOverload* overload = localVar->resolvedSymbolOverload;
        auto            typeInfo = overload->typeInfo;

        auto& loc = localVar->token.startLocation;
        if (overload->flags & OVERLOAD_RETVAL)
        {
            if (func->arg_size() > 0)
            {
                llvm::DIType*          type  = getPointerToType(typeInfo, file);
                auto                   scope = getOrCreateScope(file, localVar->ownerScope);
                llvm::DILocalVariable* var   = dbgBuilder->createParameterVariable(SP, localVar->token.ctext(), 1, file, loc.line + 1, type, !isOptimized);
                auto                   v     = func->getArg(0);
                Vector<int64_t>        expr;
                dbgBuilder->insertDeclare(v, var, dbgBuilder->createExpression(expr), debugLocGet(loc.line + 1, loc.column, scope), pp.builder->GetInsertBlock());
            }
        }
        else
        {
            llvm::DIType*          type  = getType(typeInfo, file);
            auto                   scope = getOrCreateScope(file, localVar->ownerScope);
            llvm::DILocalVariable* var   = dbgBuilder->createAutoVariable(scope, localVar->token.ctext(), file, localVar->token.startLocation.line, type, !isOptimized);
            auto                   v     = builder.CreateInBoundsGEP(stack, pp.builder->getInt32(overload->computedValue.storageOffset));
            dbgBuilder->insertDeclare(v, var, dbgBuilder->createExpression(), debugLocGet(loc.line + 1, loc.column, scope), pp.builder->GetInsertBlock());
        }
    }
}

void BackendLLVMDbg::setLocation(llvm::IRBuilder<>* builder, ByteCode* bc, ByteCodeInstruction* ip)
{
    if (bc->node && bc->node->isSpecialFunctionGenerated())
        return;

    SWAG_ASSERT(dbgBuilder);

    if (ip->node->kind == AstNodeKind::FuncDecl)
    {
        builder->SetCurrentDebugLocation(llvm::DebugLoc());
        return;
    }

    auto loc = ByteCode::getLocation(bc, ip);
    if (!loc.location)
    {
        builder->SetCurrentDebugLocation(llvm::DebugLoc());
        return;
    }

    llvm::DIFile*  file  = getOrCreateFile(bc->sourceFile);
    llvm::DIScope* scope = getOrCreateScope(file, ip->node->ownerScope);

    if (bc->sourceFile == loc.file)
    {
        lastInlineSourceFile = nullptr;
        if (lastDebugLine != loc.location->line + 1)
        {
            lastDebugLine = loc.location->line + 1;
            builder->SetCurrentDebugLocation(debugLocGet(loc.location->line + 1, 0, scope));
        }
    }
    else
    {
        llvm::DIFile* orgFile = getOrCreateFile(loc.file);
        if (loc.file != lastInlineSourceFile)
            lastInlineBlock = dbgBuilder->createLexicalBlockFile(scope, orgFile);
        else if (lastDebugLine == loc.location->line + 1)
            return;

        lastInlineSourceFile = loc.file;
        lastDebugLine        = loc.location->line + 1;
        builder->SetCurrentDebugLocation(debugLocGet(loc.location->line + 1, 0, lastInlineBlock));
    }
}

llvm::DIScope* BackendLLVMDbg::getOrCreateScope(llvm::DIFile* file, Scope* scope)
{
    SWAG_ASSERT(scope);

    // If this assert triggers, this will crash at one point in llvm. This should never
    // happens, so check the corresponding instruction !
    SWAG_ASSERT(scope->kind != ScopeKind::Function);

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

        // If parent function has not yet been created, then create it now
        if (scanScope->kind == ScopeKind::FunctionBody)
        {
            SWAG_ASSERT(scanScope->parentScope->kind == ScopeKind::Function);
            auto decl = CastAst<AstFuncDecl>(scanScope->parentScope->owner, AstNodeKind::FuncDecl);
            SWAG_ASSERT(decl->extByteCode());
            parent = startFunction(decl->extByteCode()->bc);
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
    auto& pp              = *llvm->perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;
    auto& modu            = *pp.module;
    auto  module          = llvm->module;

    VectorNative<llvm::Value*> offset;
    offset.reserve(2);
    offset.push_back(builder.getInt32(0));
    offset.push_back(builder.getInt32(0));

    VectorNative<AstNode*>* all;
    if (var == pp.bssSeg)
        all = &module->globalVarsBss;
    else if (var == pp.mutableSeg)
        all = &module->globalVarsMutable;
    else
        all = &module->globalVarsConstant;

    for (auto node : *all)
    {
        // Compile time constant
        if (node->flags & AST_VALUE_COMPUTED)
            continue;

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
        constExpr = llvm::ConstantExpr::getAdd(constExpr, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*pp.context), resolved->computedValue.storageOffset));
        // Convert back to a pointer to bytes
        constExpr = llvm::ConstantExpr::getIntToPtr(constExpr, builder.getInt8Ty()->getPointerTo());

        // Cast to the correct type
        auto varType = llvm->swagTypeToLLVMType(buildParameters, module, typeInfo);
        if (!varType)
            continue;

        varType         = varType->getPointerTo();
        constExpr       = llvm::ConstantExpr::getPointerCast(constExpr, varType);
        auto name       = node->token.ctext();
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
