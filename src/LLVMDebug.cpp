#include "pch.h"
#include "LLVMDebug.h"
#include "AstFlags.h"
#include "ByteCode.h"
#include "LLVM.h"
#include "LLVM_Macros.h"
#include "Module.h"
#include "Scope.h"
#include "TypeManager.h"
#include "Version.h"
#include "Workspace.h"

namespace
{
    llvm::DILocation* debugLocGet(unsigned Line, unsigned Col, const llvm::MDNode* Scope, const llvm::MDNode* InlinedAt = nullptr, bool ImplicitCode = false)
    {
        return llvm::DILocation::get(Scope->getContext(), Line, Col, const_cast<llvm::MDNode*>(Scope), const_cast<llvm::MDNode*>(InlinedAt), ImplicitCode);
    }
}

void LLVMDebug::setup(LLVM* m, llvm::Module* modu)
{
    llvm                = m;
    isOptimized         = !m->module->buildParameters.isDebug();
    dbgBuilder          = new llvm::DIBuilder(*modu, true);
    llvmModule          = modu;
    llvmContext         = &modu->getContext();
    mainFile            = dbgBuilder->createFile("<stdin>", "c:/");
    const Path expPath  = m->exportFilePath;
    exportFile          = dbgBuilder->createFile(m->exportFileName.string(), expPath.parent_path().string());
    const Utf8 compiler = FMT("swag %d.%d.%d", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM);
    compileUnit         = dbgBuilder->createCompileUnit(llvm::dwarf::DW_LANG_C99,
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
        auto                    v1      = dbgBuilder->createMemberType(mainFile->getScope(), "data", mainFile, 0, 64, 0, 0, llvm::DINode::DIFlags::FlagZero, ptrU8Ty);
        auto                    v2      = dbgBuilder->createMemberType(mainFile->getScope(), "sizeof", mainFile, 1, 64, 0, 64, llvm::DINode::DIFlags::FlagZero, u64Ty);
        const llvm::DINodeArray content = dbgBuilder->getOrCreateArray({v1, v2});
        stringTy                        = dbgBuilder->createStructType(mainFile->getScope(), "string", mainFile, 0, 2 * sizeof(void*) * 8, 0, llvm::DINode::DIFlags::FlagZero, nullptr, content);
    }

    // interface
    {
        auto                    v1      = dbgBuilder->createMemberType(mainFile->getScope(), "data", mainFile, 0, 64, 0, 0, llvm::DINode::DIFlags::FlagZero, ptrU8Ty);
        auto                    v2      = dbgBuilder->createMemberType(mainFile->getScope(), "itable", mainFile, 1, 64, 0, 64, llvm::DINode::DIFlags::FlagZero, ptrU8Ty);
        const llvm::DINodeArray content = dbgBuilder->getOrCreateArray({v1, v2});
        interfaceTy                     = dbgBuilder->createStructType(mainFile->getScope(), "interface", mainFile, 0, 2 * sizeof(void*) * 8, 0, llvm::DINode::DIFlags::FlagZero, nullptr, content);
    }

    // any
    {
        const auto              ptrType = getPointerToType(g_Workspace->swagScope.regTypeInfo, mainFile);
        auto                    v1      = dbgBuilder->createMemberType(mainFile->getScope(), "ptrvalue", mainFile, 0, 64, 0, 0, llvm::DINode::DIFlags::FlagZero, ptrU8Ty);
        auto                    v2      = dbgBuilder->createMemberType(mainFile->getScope(), "typeinfo", mainFile, 1, 64, 0, 64, llvm::DINode::DIFlags::FlagZero, ptrType);
        const llvm::DINodeArray content = dbgBuilder->getOrCreateArray({v1, v2});
        anyTy                           = dbgBuilder->createStructType(mainFile->getScope(), "any", mainFile, 0, 2 * sizeof(void*) * 8, 0, llvm::DINode::DIFlags::FlagZero, nullptr, content);
    }
}

llvm::DIFile* LLVMDebug::getOrCreateFile(const SourceFile* file)
{
    const auto it = mapFiles.find(file->path);
    if (it != mapFiles.end())
        return it->second;

    const Path    filePath = file->path;
    llvm::DIFile* dbgfile  = dbgBuilder->createFile(filePath.filename().string(), filePath.parent_path().string());
    mapFiles[file->path]   = dbgfile;
    return dbgfile;
}

llvm::DIType* LLVMDebug::getEnumType(TypeInfo* typeInfo, llvm::DIFile* file)
{
    const auto it = mapTypes.find(typeInfo);
    if (it != mapTypes.end())
        return it->second;

    // Normal c enum, integer only
    const auto typeInfoEnum = castTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
    if (typeInfoEnum->rawType->isNativeInteger())
    {
        const auto                    scope = file->getScope();
        VectorNative<llvm::Metadata*> subscripts;
        VectorNative<TypeInfoEnum*>   collect;
        typeInfoEnum->collectEnums(collect);
        const bool isUnsigned = typeInfoEnum->rawType->hasFlag(TYPEINFO_UNSIGNED);
        for (const auto typeEnum : collect)
        {
            for (const auto& value : typeEnum->values)
            {
                if (!value->value)
                    continue;
                const auto typeField = dbgBuilder->createEnumerator(value->name.c_str(), value->value->reg.u64, isUnsigned);
                subscripts.push_back(typeField);
            }
        }

        const auto lineNo  = typeInfo->declNode->token.startLocation.line + 1;
        const auto rawType = getType(typeInfoEnum->rawType, file);
        const auto content = dbgBuilder->getOrCreateArray({subscripts.begin(), subscripts.end()});
        typeInfo->computeScopedName();
        const auto result  = dbgBuilder->createEnumerationType(scope, typeInfo->scopedName.c_str(), file, lineNo, typeInfo->sizeOf * 8, 0, content, rawType);
        mapTypes[typeInfo] = result;
        return result;
    }

    return getType(typeInfoEnum->rawType, file);
}

llvm::DIType* LLVMDebug::getPointerToType(TypeInfo* typeInfo, llvm::DIFile* file)
{
    const auto it = mapPtrTypes.find(typeInfo);
    if (it != mapPtrTypes.end())
        return it->second;
    const auto result     = dbgBuilder->createPointerType(getType(typeInfo, file), 64);
    mapPtrTypes[typeInfo] = result;
    return result;
}

llvm::DIType* LLVMDebug::getReferenceToType(TypeInfo* typeInfo, llvm::DIFile* file)
{
    const auto it = mapRefTypes.find(typeInfo);
    if (it != mapRefTypes.end())
        return it->second;
    const auto result     = dbgBuilder->createReferenceType(llvm::dwarf::DW_TAG_reference_type, getType(typeInfo, file));
    mapRefTypes[typeInfo] = result;
    return result;
}

llvm::DIType* LLVMDebug::getStructType(TypeInfo* typeInfo, llvm::DIFile* file)
{
    const auto it = mapTypes.find(typeInfo);
    if (it != mapTypes.end())
        return it->second;

    const auto     scope  = file->getScope();
    constexpr auto noFlag = llvm::DINode::DIFlags::FlagZero;
    const auto     lineNo = typeInfo->declNode->token.startLocation.line + 1;
    typeInfo->computeScopedName();
    auto result = dbgBuilder->createStructType(scope, typeInfo->scopedName.c_str(), file, lineNo, typeInfo->sizeOf * 8, 0, noFlag, nullptr, llvm::DINodeArray(), 0, nullptr,
                                               typeInfo->scopedName.c_str());

    // Register it right away, so that even if a struct is referencing itself, this will work
    // and won't recurse forever
    mapTypes[typeInfo] = result;

    // Deal with the struct content now that the struct is registered
    VectorNative<llvm::Metadata*> subscripts;
    const auto                    typeStruct = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    for (const auto& field : typeStruct->fields)
    {
        const auto fieldLine = field->declNode->token.startLocation.line + 1;
        const auto fieldType = getType(field->typeInfo, file);
        const auto typeField = dbgBuilder->createMemberType(result, field->name.c_str(), file, fieldLine, field->typeInfo->sizeOf * 8, 0, field->offset * 8, noFlag, fieldType);
        subscripts.push_back(typeField);
    }

    const llvm::ArrayRef<llvm::Metadata*> subscripts1{subscripts.begin(), subscripts.end()};
    const auto                            content = dbgBuilder->getOrCreateArray(subscripts1);
    dbgBuilder->replaceArrays(result, content);

    return result;
}

llvm::DIType* LLVMDebug::getSliceType(TypeInfo* typeInfo, TypeInfo* pointedType, llvm::DIFile* file)
{
    const auto it = mapTypes.find(typeInfo);
    if (it != mapTypes.end())
        return it->second;

    const auto     fileScope = file->getScope();
    constexpr auto noFlag    = llvm::DINode::DIFlags::FlagZero;
    const auto     name      = FMT("[..] %s", pointedType->name.c_str()); // debugger doesn't like 'const' before slice name
    auto           result    = dbgBuilder->createStructType(fileScope, name.c_str(), file, 0, 2 * sizeof(void*) * 8, 0, noFlag, nullptr, llvm::DINodeArray());

    const auto realType = getPointerToType(pointedType, file);
    auto       v1       = dbgBuilder->createMemberType(result, "data", file, 0, 64, 0, 0, noFlag, realType);
    auto       v2       = dbgBuilder->createMemberType(result, "count", file, 1, 64, 0, 64, noFlag, u64Ty);
    const auto content  = dbgBuilder->getOrCreateArray({v1, v2});
    dbgBuilder->replaceArrays(result, content);

    mapTypes[typeInfo] = result;
    return result;
}

llvm::DIType* LLVMDebug::getType(TypeInfo* typeInfo, llvm::DIFile* file)
{
    if (!typeInfo)
        return s32Ty;
    typeInfo = typeInfo->getConcreteAlias();

    const auto it = mapTypes.find(typeInfo);
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
            const auto                    typeInfoPtr = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
            VectorNative<llvm::Metadata*> subscripts;

            auto* countNode = llvm::ConstantAsMetadata::get(llvm::ConstantInt::getSigned(llvm::Type::getInt64Ty(*llvmContext), typeInfoPtr->count));
            subscripts.push_back(dbgBuilder->getOrCreateSubrange(countNode, nullptr, nullptr, nullptr));

            const auto subscriptArray = dbgBuilder->getOrCreateArray({subscripts.begin(), subscripts.end()});
            const auto result         = dbgBuilder->createArrayType(typeInfoPtr->totalCount, 0, getType(typeInfoPtr->pointedType, file), subscriptArray);
            mapTypes[typeInfo]        = result;
            return result;
        }

        case TypeInfoKind::Pointer:
        {
            const auto    typeInfoPtr = castTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
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
            const auto typeInfoPtr = castTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
            return getSliceType(typeInfo, typeInfoPtr->pointedType, file);
        }

        case TypeInfoKind::TypedVariadic:
        {
            const auto typeInfoPtr = castTypeInfo<TypeInfoVariadic>(typeInfo, TypeInfoKind::TypedVariadic);
            return getSliceType(typeInfo, typeInfoPtr->rawType, file);
        }

        case TypeInfoKind::Variadic:
        {
            return getSliceType(typeInfo, g_TypeMgr->typeInfoAny, file);
        }

        case TypeInfoKind::LambdaClosure:
        {
            const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::LambdaClosure);
            return dbgBuilder->createPointerType(getFunctionType(typeFunc, file), 64);
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
        default:
            break;
    }

    switch (typeInfo->sizeOf)
    {
        case 1:
            return s8Ty;
        case 2:
            return s16Ty;
        case 4:
            return s32Ty;
        default:
            return s64Ty;
    }
}

llvm::DISubroutineType* LLVMDebug::getFunctionType(TypeInfoFuncAttr* typeFunc, llvm::DIFile* file)
{
    const auto it = mapFuncTypes.find(typeFunc);
    if (it != mapFuncTypes.end())
        return it->second;

    VectorNative<llvm::Metadata*> params;

    params.push_back(nullptr); // void

    if (typeFunc->returnType)
    {
        params.push_back(getType(typeFunc->returnType, file));
    }

    for (const auto one : typeFunc->parameters)
    {
        const auto typeInfo = TypeManager::concreteType(one->typeInfo);
        if (typeInfo->numRegisters() == 1)
        {
            params.push_back(getType(typeInfo, file));
        }
        else
        {
            for (uint32_t r = 0; r < one->typeInfo->numRegisters(); r++)
            {
                params.push_back(getType(g_TypeMgr->typeInfoU64, file));
            }
        }
    }

    const auto typeArray = dbgBuilder->getOrCreateTypeArray({params.begin(), params.end()});
    const auto result    = dbgBuilder->createSubroutineType(typeArray);

    mapFuncTypes[typeFunc] = result;
    return result;
}

llvm::DISubprogram* LLVMDebug::startFunction(const ByteCode* bc, AstFuncDecl** resultDecl)
{
    SWAG_ASSERT(dbgBuilder);

    Utf8              name     = bc->name;
    AstFuncDecl*      decl     = nullptr;
    int               line     = 0;
    TypeInfoFuncAttr* typeFunc = bc->typeInfoFunc;

    if (bc->node)
    {
        decl     = castAst<AstFuncDecl>(bc->node, AstNodeKind::FuncDecl);
        name     = decl->token.text;
        line     = decl->token.startLocation.line;
        typeFunc = castTypeInfo<TypeInfoFuncAttr>(decl->typeInfo, TypeInfoKind::FuncAttr);
    }

    if (resultDecl)
        *resultDecl = decl;

    // Already created ?
    const auto it = mapScopes.find(decl->content->ownerScope);
    if (it != mapScopes.end())
        return static_cast<llvm::DISubprogram*>(it->second);

    // Type
    llvm::DIFile*           file        = getOrCreateFile(bc->sourceFile);
    const unsigned          lineNo      = line + 1;
    llvm::DISubroutineType* dbgFuncType = getFunctionType(typeFunc, file);

    // Flags
    llvm::DISubprogram::DISPFlags spFlags = llvm::DISubprogram::SPFlagDefinition;
    if (isOptimized)
        spFlags |= llvm::DISubprogram::SPFlagOptimized;
    if (!decl || !decl->hasAttribute(ATTRIBUTE_PUBLIC))
        spFlags |= llvm::DISubprogram::SPFlagLocalToUnit;

    constexpr llvm::DINode::DIFlags diFlags = llvm::DINode::FlagPrototyped | llvm::DINode::FlagStaticMember;

    // Register function
    const auto          mangledName = typeFunc->declNode->getScopedName();
    llvm::DISubprogram* SP          = dbgBuilder->createFunction(file, name.c_str(), mangledName.c_str(), file, lineNo, dbgFuncType, lineNo, diFlags, spFlags);
    if (decl)
        mapScopes[decl->content->ownerScope] = SP;

    return SP;
}

void LLVMDebug::startFunction(const BuildParameters& buildParameters, const LLVMEncoder& pp, ByteCode* bc, llvm::Function* func, llvm::AllocaInst* stack)
{
    if (bc->node && bc->node->isSpecialFunctionGenerated())
        return;

    auto& builder = *pp.builder;
    auto& context = *pp.llvmContext;

    builder.SetCurrentDebugLocation(llvm::DebugLoc());
    lastDebugLine        = 0;
    lastInlineSourceFile = nullptr;

    AstFuncDecl*        decl;
    llvm::DISubprogram* SP = startFunction(bc, &decl);
    func->setSubprogram(SP);

    TypeInfoFuncAttr* typeFunc = bc->typeInfoFunc;
    llvm::DIFile*     file     = getOrCreateFile(bc->sourceFile);
    if (decl)
        typeFunc = castTypeInfo<TypeInfoFuncAttr>(decl->typeInfo, TypeInfoKind::FuncAttr);

    VectorNative<llvm::AllocaInst*> allocaParams;
    VectorNative<llvm::AllocaInst*> allocaRetval;
    llvm::AllocaInst*               allocaVariadic = nullptr;
    size_t                          countParams    = 0;
    int                             idxParam       = 0;
    const bool                      isDebug        = buildParameters.isDebug();

    // Allocate some temporary variables linked to parameters
    if (decl && decl->parameters && !decl->hasAttribute(ATTRIBUTE_COMPILER_FUNC))
    {
        countParams = decl->parameters->children.size();
        allocaParams.reserve(static_cast<uint32_t>(countParams));

        if (typeFunc->hasFlag(TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
        {
            allocaVariadic = builder.CreateAlloca(I64_TY(), builder.getInt64(2));
            idxParam += 2;
            countParams--;
        }
        else if (typeFunc->hasFlag(TYPEINFO_C_VARIADIC))
        {
            countParams--;
        }

        for (size_t i = 0; i < countParams; i++)
        {
            const auto typeParam = typeFunc->parameters[i]->typeInfo;
            if (typeParam->isString() || typeParam->isSlice())
            {
                auto allocA = builder.CreateAlloca(I64_TY(), builder.getInt64(2));
                allocA->setAlignment(llvm::Align{16});
                allocaParams.push_back(allocA);
            }

            // :OptimizedAwayDebugCrap
            // Parameters are "optimized away" by the debugger, most of the time.
            // The only way to have correct values seems to make a local copy of the parameter on the stack,
            // and make the debugger use that copy instead of the parameter.
            // So make a copy, except if we are in optimized mode...
            else if (isDebug)
            {
                auto allocA = builder.CreateAlloca(func->getArg(idxParam)->getType(), nullptr, func->getArg(idxParam)->getName());
                allocA->setAlignment(llvm::Align{16});
                allocaParams.push_back(allocA);
            }
            else
            {
                allocaParams.push_back(nullptr);
            }
        }
    }

    // Allocate some temporary variables for each local 'retval', in order to make a copy of the return pointer
    if (func->arg_size() > 0)
    {
        for (const auto localVar : bc->localVars)
        {
            const SymbolOverload* overload = localVar->resolvedSymbolOverload();
            if (overload->node->hasAstFlag(AST_GENERATED))
                continue;

            if (overload->hasFlag(OVERLOAD_RETVAL))
            {
                auto allocA = builder.CreateAlloca(I64_TY(), builder.getInt64(1));
                allocA->setAlignment(llvm::Align{16});
                allocaRetval.push_back(allocA);
            }
        }
    }

    if (decl && decl->parameters && !decl->hasAttribute(ATTRIBUTE_COMPILER_FUNC))
    {
        // Variadic. Pass as first parameters, but get type at the end
        if (typeFunc->hasFlag(TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
        {
            const auto    child     = decl->parameters->children.back();
            const auto&   loc       = child->token.startLocation;
            const auto    typeParam = typeFunc->parameters.back()->typeInfo;
            const auto    scope     = SP;
            llvm::DIType* type      = getType(typeParam, file);

            llvm::DILocalVariable* var0 = dbgBuilder->createAutoVariable(scope, child->token.c_str(), file, loc.line + 1, type, !isOptimized);

            const auto v0   = builder.CreateInBoundsGEP(I64_TY(), allocaVariadic, builder.getInt64(0));
            const auto arg0 = func->getArg(0);
            builder.CreateStore(arg0, builder.CreatePointerCast(v0, arg0->getType()->getPointerTo()));

            const auto v1 = builder.CreateInBoundsGEP(I64_TY(), allocaVariadic, builder.getInt64(1));
            builder.CreateStore(func->getArg(1), v1);

            dbgBuilder->insertDeclare(allocaVariadic, var0, dbgBuilder->createExpression(), debugLocGet(loc.line + 1, loc.column, scope), pp.builder->GetInsertBlock());
        }

        for (size_t i = 0; i < countParams; i++)
        {
            const auto  child     = decl->parameters->children[i];
            const auto& loc       = child->token.startLocation;
            const auto  typeParam = typeFunc->parameters[i]->typeInfo;
            const auto  scope     = SP;
            const auto  location  = debugLocGet(loc.line + 1, loc.column, scope);
            const auto  allocA    = allocaParams[i];

            // If parameters are passed with two registers, make a local variable instead of a reference to
            // the parameters
            if (typeParam->isString() || typeParam->isSlice())
            {
                const auto             type  = getType(typeParam, file);
                llvm::DILocalVariable* value = dbgBuilder->createAutoVariable(scope, child->token.c_str(), file, loc.line + 1, type, !isOptimized);

                if (isDebug)
                {
                    const auto v0   = builder.CreateInBoundsGEP(I64_TY(), allocA, builder.getInt64(0));
                    const auto arg0 = func->getArg(idxParam);
                    builder.CreateStore(arg0, builder.CreatePointerCast(v0, arg0->getType()->getPointerTo()));

                    const auto v1 = builder.CreateInBoundsGEP(I64_TY(), allocA, builder.getInt64(1));
                    builder.CreateStore(func->getArg(idxParam + 1), v1);
                }

                dbgBuilder->insertDeclare(allocA, value, dbgBuilder->createExpression(), debugLocGet(loc.line + 1, loc.column, scope), pp.builder->GetInsertBlock());
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

                llvm::DINode::DIFlags flags = llvm::DINode::FlagZero;
                if (typeParam->isSelf() && child->token.text == "self")
                    flags |= llvm::DINode::FlagObjectPointer;

                llvm::DILocalVariable* var = dbgBuilder->createParameterVariable(scope, child->token.c_str(), idxParam + 1, file, loc.line + 1, type, !isOptimized, flags);

                llvm::Value* value = func->getArg(idxParam);

                if (isDebug)
                {
                    builder.CreateStore(value, allocA);
                    value = allocA;
                }

                dbgBuilder->insertDeclare(value, var, dbgBuilder->createExpression(), location, pp.builder->GetInsertBlock());
            }

            idxParam += typeParam->numRegisters();
        }
    }

    // Local variables
    uint32_t idxRetVal = 0;
    for (const auto localVar : bc->localVars)
    {
        const SymbolOverload* overload = localVar->resolvedSymbolOverload();
        if (overload->node->hasAstFlag(AST_GENERATED))
            continue;

        const auto  typeInfo = overload->typeInfo;
        const auto& loc      = localVar->token.startLocation;

        if (overload->hasFlag(OVERLOAD_RETVAL) && idxRetVal < allocaRetval.size())
        {
            llvm::DIType*          type   = getPointerToType(typeInfo, file);
            const auto             scope  = getOrCreateScope(file, localVar->ownerScope);
            llvm::DILocalVariable* var    = dbgBuilder->createAutoVariable(scope, localVar->token.c_str(), file, localVar->token.startLocation.line, type, !isOptimized);
            const auto             allocA = allocaRetval[idxRetVal++];
            dbgBuilder->insertDeclare(allocA, var, dbgBuilder->createExpression(), debugLocGet(loc.line + 1, loc.column, scope), pp.builder->GetInsertBlock());
            builder.CreateStore(func->getArg(static_cast<uint32_t>(func->arg_size()) - 1), allocA);
        }
        else
        {
            llvm::DIType*          type  = getType(typeInfo, file);
            const auto             scope = getOrCreateScope(file, localVar->ownerScope);
            llvm::DILocalVariable* var   = dbgBuilder->createAutoVariable(scope, localVar->token.c_str(), file, localVar->token.startLocation.line, type, !isOptimized);
            const auto             v     = builder.CreateInBoundsGEP(I8_TY(), stack, pp.builder->getInt32(overload->computedValue.storageOffset));
            dbgBuilder->insertDeclare(v, var, dbgBuilder->createExpression(), debugLocGet(loc.line + 1, loc.column, scope), pp.builder->GetInsertBlock());
        }
    }
}

void LLVMDebug::setLocation(llvm::IRBuilder<>* builder, const ByteCode* bc, const ByteCodeInstruction* ip)
{
    if (bc->node && bc->node->isSpecialFunctionGenerated())
        return;

    SWAG_ASSERT(dbgBuilder);

    if (ip->node->kind == AstNodeKind::FuncDecl)
    {
        builder->SetCurrentDebugLocation(llvm::DebugLoc());
        return;
    }

    const auto loc = ByteCode::getLocation(bc, ip, bc->sourceFile && bc->sourceFile->module->buildCfg.backendDebugInline);
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

llvm::DIScope* LLVMDebug::getOrCreateScope(llvm::DIFile* file, Scope* scope)
{
    SWAG_ASSERT(scope);

    // If this asserts triggers, this will crash at one point in llvm. This should never
    // happen, so check the corresponding instruction !
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
            const auto decl = castAst<AstFuncDecl>(scanScope->parentScope->owner, AstNodeKind::FuncDecl);
            SWAG_ASSERT(decl->hasExtByteCode());
            parent = startFunction(decl->extByteCode()->bc);
            break;
        }

        toGen.push_back(scanScope);
        scanScope = scanScope->parentScope;
    }

    if (toGen.empty())
        return parent;

    for (int i = static_cast<int>(toGen.size()) - 1; i >= 0; i--)
    {
        auto           toGenScope = toGen[i];
        llvm::DIScope* newScope;
        if (toGenScope->kind == ScopeKind::Namespace)
        {
            newScope = dbgBuilder->createNameSpace(parent, toGenScope->name.c_str(), true);
        }
        else if (toGenScope->isGlobal())
        {
            newScope = dbgBuilder->createLexicalBlockFile(parent, file);
        }
        else
        {
            const auto lineNo = toGenScope->owner ? toGenScope->owner->token.startLocation.line + 1 : 0;
            newScope          = dbgBuilder->createLexicalBlock(parent, file, lineNo, 0);
        }
        mapScopes[toGenScope] = newScope;
        parent                = newScope;
    }

    return parent;
}

void LLVMDebug::createGlobalVariablesForSegment(const BuildParameters& buildParameters, llvm::Type* type, llvm::GlobalVariable* var)
{
    const auto  ct              = buildParameters.compileType;
    const auto  precompileIndex = buildParameters.precompileIndex;
    const auto& pp              = llvm->encoder<LLVMEncoder>(ct, precompileIndex);
    auto&       builder         = *pp.builder;
    auto&       context         = *pp.llvmContext;
    auto&       modu            = *pp.llvmModule;
    const auto  module          = llvm->module;

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

    for (const auto node : *all)
    {
        const auto resolved = node->resolvedSymbolOverload();
        const auto typeInfo = node->typeInfo;
        const auto file     = compileUnit->getFile();
        const auto dbgType  = getType(typeInfo, file);
        if (!dbgType)
            continue;

        // Cast to the correct type
        auto varType = llvm->swagTypeToLLVMType(buildParameters, typeInfo);
        if (!varType)
            continue;

        auto       nameU = node->getScopedName();
        const auto name  = nameU.c_str();

        if (node->hasFlagComputedValue())
        {
            llvm::Constant* constant = nullptr;
            if (typeInfo->isNative())
            {
                switch (typeInfo->nativeType)
                {
                    case NativeTypeKind::S8:
                        constant = llvm::ConstantInt::get(I8_TY(), node->computedValue()->reg.s32, true);
                        break;
                    case NativeTypeKind::S16:
                        constant = llvm::ConstantInt::get(I8_TY(), node->computedValue()->reg.s16, true);
                        break;
                    case NativeTypeKind::S32:
                        constant = llvm::ConstantInt::get(I32_TY(), node->computedValue()->reg.s32, true);
                        break;
                    case NativeTypeKind::S64:
                        constant = llvm::ConstantInt::get(I32_TY(), node->computedValue()->reg.s64, true);
                        break;
                    case NativeTypeKind::U8:
                        constant = llvm::ConstantInt::get(I8_TY(), node->computedValue()->reg.s32, false);
                        break;
                    case NativeTypeKind::U16:
                        constant = llvm::ConstantInt::get(I8_TY(), node->computedValue()->reg.s16, false);
                        break;
                    case NativeTypeKind::U32:
                    case NativeTypeKind::Rune:
                        constant = llvm::ConstantInt::get(I32_TY(), node->computedValue()->reg.s32, false);
                        break;
                    case NativeTypeKind::U64:
                        constant = llvm::ConstantInt::get(I32_TY(), node->computedValue()->reg.s64, false);
                        break;
                    case NativeTypeKind::F32:
                        constant = llvm::ConstantFP::get(F32_TY(), node->computedValue()->reg.f32);
                        break;
                    case NativeTypeKind::F64:
                        constant = llvm::ConstantFP::get(F64_TY(), node->computedValue()->reg.f64);
                        break;
                    case NativeTypeKind::Bool:
                        constant = llvm::ConstantInt::get(I1_TY(), node->computedValue()->reg.b, false);
                        break;
                    default:
                        break;
                }
            }

            if (constant)
            {
                const auto g   = new llvm::GlobalVariable(modu, constant->getType(), true, llvm::GlobalValue::PrivateLinkage, constant, name);
                const auto gve = pp.dbg->dbgBuilder->createGlobalVariableExpression(compileUnit, node->token.c_str(), name, file, 1, dbgType, true);
                g->addDebugInfo(gve);
            }
        }
        else
        {
            // Segment is stored as an array of 64 bits elements. Get address of the first element
            auto constExpr = llvm::ConstantExpr::getGetElementPtr(type, var, {offset.begin(), offset.end()});
            // Convert to a pointer to bytes
            constExpr = llvm::ConstantExpr::getPointerCast(constExpr, PTR_I8_TY());
            // Convert to int, in order to make an Add
            constExpr = llvm::ConstantExpr::getPtrToInt(constExpr, I64_TY());
            // Add the byte offset
            constExpr = llvm::ConstantExpr::getAdd(constExpr, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*pp.llvmContext), resolved->computedValue.storageOffset));
            // Convert back to a pointer to bytes
            constExpr = llvm::ConstantExpr::getIntToPtr(constExpr, PTR_I8_TY());

            varType   = varType->getPointerTo();
            constExpr = llvm::ConstantExpr::getPointerCast(constExpr, varType);

            const auto g          = new llvm::GlobalVariable(modu, varType, false, llvm::GlobalValue::ExternalLinkage, constExpr, name);
            const auto dbgRefType = getReferenceToType(typeInfo, file);
            const auto gve        = pp.dbg->dbgBuilder->createGlobalVariableExpression(compileUnit, node->token.c_str(), name, file, 0, dbgRefType, true);
            g->addDebugInfo(gve);
        }
    }
}

void LLVMDebug::finalize() const
{
    SWAG_ASSERT(dbgBuilder);
    dbgBuilder->finalize();
}
