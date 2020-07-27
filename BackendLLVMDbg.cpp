#include "pch.h"
#include "BackendLLVMDbg.h"
#include "ByteCode.h"
#include "SourceFile.h"
#include "Ast.h"

void BackendLLVMDbg::setup(llvm::Module* modu)
{
    dbgBuilder    = new llvm::DIBuilder(*modu);
    auto mainFile = dbgBuilder->createFile("module.pdb", "f:/");
    compileUnit   = dbgBuilder->createCompileUnit(llvm::dwarf::DW_LANG_C, mainFile, "Swag Compiler", 0, "", 0);
    modu->addModuleFlag(llvm::Module::Warning, "Debug Info Version", llvm::DEBUG_METADATA_VERSION);
#ifdef _WIN32
    modu->addModuleFlag(llvm::Module::Warning, "CodeView", llvm::DEBUG_METADATA_VERSION);
#endif
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
    auto dblTy = dbgBuilder->createBasicType("s32", 32, llvm::dwarf::DW_ATE_signed);
    return dblTy;
}

llvm::DISubroutineType* BackendLLVMDbg::createFunctionType(TypeInfoFuncAttr* typeFunc)
{
    vector<llvm::Metadata*> params;
    auto                    dblTy = dbgBuilder->createBasicType("double", 64, llvm::dwarf::DW_ATE_float);
    params.push_back(dblTy);

    auto typeArray = dbgBuilder->getOrCreateTypeArray(params);
    return dbgBuilder->createSubroutineType(typeArray);
}

void BackendLLVMDbg::startFunction(ByteCode* bc, llvm::Function* func)
{
    if (!dbgBuilder)
        return;

    Utf8 name = bc->name;
    int  line = 0;
    if (bc->node)
    {
        AstFuncDecl* decl = CastAst<AstFuncDecl>(bc->node, AstNodeKind::FuncDecl);
        name              = decl->name;
        line              = decl->token.startLocation.line;
    }

    llvm::DIFile*           file        = getOrCreateFile(bc->sourceFile);
    unsigned                lineNo      = line + 1;
    llvm::DISubroutineType* dbgFuncType = createFunctionType(bc->typeInfoFunc);
    llvm::DISubprogram*     SP          = dbgBuilder->createFunction(compileUnit->getFile(),
                                                        name.c_str(),
                                                        name.c_str(),
                                                        file,
                                                        lineNo,
                                                        dbgFuncType,
                                                        lineNo,
                                                        llvm::DINode::FlagPrototyped,
                                                        llvm::DISubprogram::SPFlagDefinition);
    func->setSubprogram(SP);

    scopes.push_back(SP);
}

void BackendLLVMDbg::createLocalVar(llvm::IRBuilder<>* builder, llvm::Value* storage, ByteCodeInstruction* ip)
{
    if (!dbgBuilder)
        return;

    auto node = ip->node;
    SWAG_ASSERT(node);
    SymbolOverload* overload = node->resolvedSymbolOverload;
    SWAG_ASSERT(overload);
    auto typeInfo = overload->typeInfo;
    SWAG_ASSERT(typeInfo);

    llvm::DIType*          type = getType(typeInfo);
    llvm::DILocalVariable* var  = dbgBuilder->createAutoVariable(scopes.back(),
                                                                node->name.c_str(),
                                                                scopes.back()->getFile(),
                                                                node->token.startLocation.line,
                                                                type);

    auto v = builder->CreateInBoundsGEP(storage, builder->getInt32(overload->storageOffset));
    dbgBuilder->insertDeclare(v,
                              var,
                              dbgBuilder->createExpression(),
                              llvm::DebugLoc::get(node->token.startLocation.line + 1, node->token.startLocation.column, scopes.back()),
                              builder->GetInsertBlock());
}

void BackendLLVMDbg::endFunction()
{
    if (!dbgBuilder)
        return;
    scopes.pop_back();
}

void BackendLLVMDbg::finalize()
{
    if (!dbgBuilder)
        return;
    dbgBuilder->finalize();
}

void BackendLLVMDbg::setLocation(llvm::IRBuilder<>* builder, AstNode* node)
{
    if (!dbgBuilder)
        return;

    if (!node)
        builder->SetCurrentDebugLocation(nullptr);
    else
        builder->SetCurrentDebugLocation(llvm::DebugLoc::get(node->token.startLocation.line + 1, 0 /*node->token.startLocation.column*/, scopes.back()));
}

void BackendLLVMDbg::pushLexicalScope(AstNode* node)
{
    if (!dbgBuilder)
        return;
    auto scope = dbgBuilder->createLexicalBlock(scopes.back(), scopes.back()->getFile(), node->token.startLocation.line + 1, 0 /*node->token.startLocation.column*/);
    scopes.push_back(scope);
}

void BackendLLVMDbg::popLexicalScope()
{
    scopes.pop_back();
}
