#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVMDbg.h"
#include "ByteCode.h"
#include "SourceFile.h"
#include "Ast.h"
#include "TypeInfo.h"
#include "BackendLLVM.h"

void BackendLLVMDbg::setup(llvm::Module* modu)
{
    dbgBuilder    = new llvm::DIBuilder(*modu);
    auto mainFile = dbgBuilder->createFile("module.pdb", "f:/");
    compileUnit   = dbgBuilder->createCompileUnit(llvm::dwarf::DW_LANG_C, mainFile, "Swag Compiler", 0, "", 0);
    modu->addModuleFlag(llvm::Module::Warning, "Debug Info Version", llvm::DEBUG_METADATA_VERSION);
#ifdef _WIN32
    modu->addModuleFlag(llvm::Module::Warning, "CodeView", llvm::DEBUG_METADATA_VERSION);
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

    {
        auto              v1      = dbgBuilder->createMemberType(mainFile->getScope(), "data", mainFile, 0, 64, 0, 0, llvm::DINode::DIFlags::FlagZero, ptrU8Ty);
        auto              v2      = dbgBuilder->createMemberType(mainFile->getScope(), "sizeof", mainFile, 1, 32, 0, 64, llvm::DINode::DIFlags::FlagZero, u32Ty);
        llvm::DINodeArray content = dbgBuilder->getOrCreateArray({v1, v2});
        stringTy                  = dbgBuilder->createStructType(mainFile->getScope(),
                                                "string",
                                                mainFile,
                                                0,
                                                2 * sizeof(void*),
                                                0,
                                                llvm::DINode::DIFlags::FlagZero,
                                                nullptr,
                                                content);
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

llvm::DIType* BackendLLVMDbg::getType(TypeInfo* typeInfo)
{
    if (!typeInfo)
        return s32Ty;

    switch (typeInfo->kind)
    {
    case TypeInfoKind::Array:
    {
        auto                    typeInfoPtr = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        vector<llvm::Metadata*> subscripts;
        subscripts.push_back(dbgBuilder->getOrCreateSubrange(0, typeInfoPtr->count));
        llvm::DINodeArray subscriptArray = dbgBuilder->getOrCreateArray(subscripts);
        return dbgBuilder->createArrayType(typeInfoPtr->totalCount, 0, getType(typeInfoPtr->pointedType), subscriptArray);
    }
    case TypeInfoKind::Pointer:
    {
        auto typeInfoPtr = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        return dbgBuilder->createPointerType(getType(typeInfoPtr->pointedType), 64);
    }
    case TypeInfoKind::Reference:
    {
        auto typeInfoPtr = CastTypeInfo<TypeInfoReference>(typeInfo, TypeInfoKind::Reference);
        return dbgBuilder->createPointerType(getType(typeInfoPtr->pointedType), 64);
    }
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

llvm::DISubroutineType* BackendLLVMDbg::createFunctionType(TypeInfoFuncAttr* typeFunc)
{
    vector<llvm::Metadata*> params;

    params.push_back(getType(typeFunc->returnType));
    for (auto one : typeFunc->parameters)
        params.push_back(getType(one->typeInfo));

    auto typeArray = dbgBuilder->getOrCreateTypeArray(params);
    return dbgBuilder->createSubroutineType(typeArray);
}

void BackendLLVMDbg::startFunction(LLVMPerThread& pp, ByteCode* bc, llvm::Function* func)
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

    // Register function
    llvm::DIFile*           file        = getOrCreateFile(bc->sourceFile);
    unsigned                lineNo      = line + 1;
    llvm::DISubroutineType* dbgFuncType = createFunctionType(typeFunc);
    llvm::DISubprogram*     SP          = dbgBuilder->createFunction(compileUnit->getFile(), name.c_str(), name.c_str(), file, lineNo, dbgFuncType, lineNo, llvm::DINode::FlagPrototyped, llvm::DISubprogram::SPFlagDefinition);
    func->setSubprogram(SP);
    scopes.push_back(SP);

    // Parameters
    if (decl && decl->parameters)
    {
        auto idxParam = typeFunc->numReturnRegisters();
        for (int i = 0; i < decl->parameters->childs.size(); i++)
        {
            auto          child = decl->parameters->childs[i];
            const auto&   loc   = child->token.startLocation;
            llvm::DIType* type  = dbgFuncType->getTypeArray()[i + 1];

            // Transform an array to a pointer in case of a function parameter
            if (typeFunc->parameters[i]->typeInfo->kind == TypeInfoKind::Array)
                type = dbgBuilder->createPointerType(type, 60);

            llvm::DILocalVariable* var = dbgBuilder->createParameterVariable(SP, child->name.c_str(), i + 1, file, loc.line + 1, type);
            dbgBuilder->insertDeclare(func->getArg(idxParam), var, dbgBuilder->createExpression(), llvm::DebugLoc::get(loc.line + 1, loc.column, scopes.back()), &func->getEntryBlock());
            idxParam += child->typeInfo->numRegisters();
        }
    }
}

void BackendLLVMDbg::createLocalVar(LLVMPerThread& pp, llvm::Function* func, llvm::Value* storage, ByteCodeInstruction* ip)
{
    SWAG_ASSERT(dbgBuilder);
    SWAG_ASSERT(ip->node);

    auto            node     = ip->node;
    SymbolOverload* overload = node->resolvedSymbolOverload;
    auto            typeInfo = overload->typeInfo;

    llvm::DIType*          type = getType(typeInfo);
    llvm::DILocalVariable* var  = dbgBuilder->createAutoVariable(scopes.back(), node->name.c_str(), scopes.back()->getFile(), node->token.startLocation.line, type);

    auto&             loc     = node->token.startLocation;
    llvm::IRBuilder<> builder(&func->getEntryBlock(), func->getEntryBlock().begin());
    auto              v = GEP_I32(storage, overload->storageOffset);
    dbgBuilder->insertDeclare(v, var, dbgBuilder->createExpression(), llvm::DebugLoc::get(loc.line + 1, loc.column, scopes.back()), builder.GetInsertBlock());
}

void BackendLLVMDbg::endFunction()
{
    SWAG_ASSERT(dbgBuilder);
    scopes.pop_back();
}

void BackendLLVMDbg::finalize()
{
    SWAG_ASSERT(dbgBuilder);
    dbgBuilder->finalize();
}

void BackendLLVMDbg::setLocation(llvm::IRBuilder<>* builder, ByteCodeInstruction* ip)
{
    SWAG_ASSERT(dbgBuilder);
    if (!ip || !ip->location)
        builder->SetCurrentDebugLocation(nullptr);
    else
        builder->SetCurrentDebugLocation(llvm::DebugLoc::get(ip->location->line + 1, 0 /*ip->location->column*/, scopes.back()));
}

void BackendLLVMDbg::pushLexicalScope(AstNode* node)
{
    SWAG_ASSERT(dbgBuilder);
    auto scope = dbgBuilder->createLexicalBlock(scopes.back(), scopes.back()->getFile(), node->token.startLocation.line + 1, 0 /*node->token.startLocation.column*/);
    scopes.push_back(scope);
}

void BackendLLVMDbg::popLexicalScope()
{
    scopes.pop_back();
}
