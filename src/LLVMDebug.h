#pragma once

struct AstFuncDecl;
struct LLVM;
struct BuildParameters;
struct ByteCode;
struct ByteCodeInstruction;
struct LLVMEncoder;
struct Scope;
struct SourceFile;
struct TypeInfo;
struct TypeInfoFuncAttr;

struct LLVMDebug
{
    void                    setup(LLVM* m, llvm::Module* modu);
    llvm::DISubprogram*     startFunction(const ByteCode* bc, AstFuncDecl** resultDecl = nullptr);
    void                    startFunction(const BuildParameters& buildParameters, const LLVMEncoder& pp, ByteCode* bc, llvm::Function* func, llvm::AllocaInst* stack);
    void                    finalize() const;
    void                    setLocation(llvm::IRBuilder<>* builder, const ByteCode* bc, const ByteCodeInstruction* ip);
    void                    createGlobalVariablesForSegment(const BuildParameters& buildParameters, llvm::Type* type, llvm::GlobalVariable* var);
    llvm::DIFile*           getOrCreateFile(const SourceFile* file);
    llvm::DIType*           getSliceType(TypeInfo* typeInfo, TypeInfo* pointedType, llvm::DIFile* file);
    llvm::DIType*           getEnumType(TypeInfo* typeInfo, llvm::DIFile* file);
    llvm::DIType*           getPointerToType(TypeInfo* typeInfo, llvm::DIFile* file);
    llvm::DIType*           getReferenceToType(TypeInfo* typeInfo, llvm::DIFile* file);
    llvm::DIType*           getStructType(TypeInfo* typeInfo, llvm::DIFile* file);
    llvm::DIType*           getType(TypeInfo* typeInfo, llvm::DIFile* file);
    llvm::DISubroutineType* getFunctionType(TypeInfoFuncAttr* typeFunc, llvm::DIFile* file);
    llvm::DIScope*          getOrCreateScope(llvm::DIFile* file, Scope* scope);

    LLVM*                llvm        = nullptr;
    llvm::LLVMContext*   llvmContext = nullptr;
    llvm::Module*        llvmModule  = nullptr;
    llvm::DIBuilder*     dbgBuilder  = nullptr;
    llvm::DICompileUnit* compileUnit = nullptr;
    llvm::DIFile*        mainFile    = nullptr;
    llvm::DIFile*        exportFile  = nullptr;
    bool                 isOptimized = false;

    SourceFile*               lastInlineSourceFile = nullptr;
    llvm::DILexicalBlockFile* lastInlineBlock      = nullptr;
    uint32_t                  lastDebugLine        = 0;

    MapPath<llvm::DIFile*>                  mapFiles;
    Map<TypeInfo*, llvm::DIType*>           mapTypes;
    Map<TypeInfo*, llvm::DIType*>           mapPtrTypes;
    Map<TypeInfo*, llvm::DIType*>           mapRefTypes;
    Map<TypeInfo*, llvm::DISubroutineType*> mapFuncTypes;
    Map<Scope*, llvm::DIScope*>             mapScopes;

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
    llvm::DICompositeType* interfaceTy;
    llvm::DICompositeType* anyTy;
};
