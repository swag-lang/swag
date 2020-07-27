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
    llvm::DISubprogram*     SP          = dbgBuilder->createFunction(
        compileUnit->getFile(),
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