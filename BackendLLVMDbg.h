#pragma once
struct ByteCode;
struct SourceFile;
struct TypeInfoFuncAttr;
struct AstNode;
struct TypeInfo;
struct ByteCodeInstruction;
struct LLVMPerThread;
struct BackendLLVM;
struct BuildParameters;
struct Scope;

struct BackendLLVMDbg
{
    void setup(BackendLLVM* m, llvm::Module* module);
    void startFunction(LLVMPerThread& pp, ByteCode* bc, llvm::Function* func, llvm::AllocaInst* stack);
    void finalize();
    void setLocation(llvm::IRBuilder<>* builder, ByteCode* bc, ByteCodeInstruction* ip);
    void createLocalVar(LLVMPerThread& pp, llvm::DIFile* file, llvm::Function* func, llvm::Value* storage, AstNode* node);
    void createGlobalVariablesForSegment(const BuildParameters& buildParameters, llvm::Type* type, llvm::GlobalVariable* var);

    llvm::DIFile* getOrCreateFile(SourceFile* file);

    llvm::DIType*           getSliceType(TypeInfo* typeInfo, llvm::DIFile* file);
    llvm::DIType*           getEnumType(TypeInfo* typeInfo, llvm::DIFile* file);
    llvm::DIType*           getPointerToType(TypeInfo* typeInfo, llvm::DIFile* file);
    llvm::DIType*           getReferenceToType(TypeInfo* typeInfo, llvm::DIFile* file);
    llvm::DIType*           getStructType(TypeInfo* typeInfo, llvm::DIFile* file);
    llvm::DIType*           getType(TypeInfo* typeInfo, llvm::DIFile* file);
    llvm::DISubroutineType* getFunctionType(TypeInfoFuncAttr* typeFunc, llvm::DIFile* file);
    llvm::DIScope*          getOrCreateScope(llvm::DIFile* file, Scope* scope);

    BackendLLVM*         llvm        = nullptr;
    llvm::LLVMContext*   llvmContext = nullptr;
    llvm::Module*        llvmModule  = nullptr;
    llvm::DIBuilder*     dbgBuilder  = nullptr;
    llvm::DICompileUnit* compileUnit = nullptr;

    map<string, llvm::DIFile*>              mapFiles;
    map<TypeInfo*, llvm::DIType*>           mapTypes;
    map<TypeInfo*, llvm::DIType*>           mapPtrTypes;
    map<TypeInfo*, llvm::DIType*>           mapRefTypes;
    map<TypeInfo*, llvm::DISubroutineType*> mapFuncTypes;
    map<Scope*, llvm::DIScope*>             mapScopes;

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
