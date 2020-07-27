#pragma once
struct ByteCode;
struct SourceFile;
struct TypeInfoFuncAttr;
struct AstNode;

struct BackendLLVMDbg
{
    void setup(llvm::Module* module);
    void startFunction(ByteCode* bc, llvm::Function* func);
    void endFunction();
    void finalize();
    void setLocation(llvm::IRBuilder<>* builder, AstNode* node);
    void pushLexicalScope(AstNode* node);
    void popLexicalScope();

    llvm::DIFile*           getOrCreateFile(SourceFile* file);
    llvm::DISubroutineType* createFunctionType(TypeInfoFuncAttr* typeFunc);

    llvm::DIBuilder*       dbgBuilder  = nullptr;
    llvm::DICompileUnit*   compileUnit = nullptr;
    vector<llvm::DIScope*> scopes;

    map<string, llvm::DIFile*> mapFiles;
};
