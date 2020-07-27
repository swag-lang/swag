#pragma once
struct ByteCode;
struct SourceFile;
struct TypeInfoFuncAttr;
struct AstNode;
struct TypeInfo;
struct ByteCodeInstruction;

struct BackendLLVMDbg
{
    void setup(llvm::Module* module);
    void startFunction(ByteCode* bc, llvm::Function* func);
    void endFunction();
    void finalize();
    void setLocation(llvm::IRBuilder<>* builder, ByteCodeInstruction* ip);
    void pushLexicalScope(AstNode* node);
    void popLexicalScope();
    void createLocalVar(llvm::IRBuilder<>* builder, llvm::Value* storage, ByteCodeInstruction* ip);

    llvm::DIFile*           getOrCreateFile(SourceFile* file);
    llvm::DIType*           getType(TypeInfo* typeInfo);
    llvm::DISubroutineType* createFunctionType(TypeInfoFuncAttr* typeFunc);

    llvm::DIBuilder*       dbgBuilder  = nullptr;
    llvm::DICompileUnit*   compileUnit = nullptr;
    vector<llvm::DIScope*> scopes;

    map<string, llvm::DIFile*> mapFiles;

    llvm::DIType*          s8Ty;
    llvm::DIType*          s16Ty;
    llvm::DIType*          s32Ty;
    llvm::DIType*          s64Ty;
    llvm::DIType*          u8Ty;
    llvm::DIType*          u16Ty;
    llvm::DIType*          u32Ty;
    llvm::DIType*          u64Ty;
    llvm::DIType*          f32Ty;
    llvm::DIType*          f64Ty;
    llvm::DIType*          boolTy;
    llvm::DIType*          charTy;
    llvm::DIDerivedType*   ptrU8Ty;
    llvm::DICompositeType* stringTy;
};
