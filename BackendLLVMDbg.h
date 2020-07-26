#pragma once
struct ByteCode;
struct SourceFile;
struct TypeInfoFuncAttr;

struct BackendLLVMDbg
{
    void setup(llvm::Module* module);
    void addFunction(ByteCode* bc, llvm::Function* func);
    void finalize();

    llvm::DIFile*           getOrCreateFile(SourceFile* file);
    llvm::DISubroutineType* createFunctionType(TypeInfoFuncAttr* typeFunc);

    llvm::DIBuilder*           dbgBuilder  = nullptr;
    llvm::DICompileUnit*       compileUnit = nullptr;
    map<string, llvm::DIFile*> mapFiles;
};
