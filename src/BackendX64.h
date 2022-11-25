#pragma once
#include "Utf8.h"
#include "Backend.h"
#include "BackendParameters.h"
#include "DataSegment.h"
#include "X64Gen.h"

struct Module;
struct BuildParameters;
struct Job;
struct DataSegment;
struct ByteCode;
struct TypeInfo;
struct ByteCodeInstruction;

struct BackendX64 : public Backend
{
    BackendX64(Module* mdl)
        : Backend{mdl}
    {
        memset(perThread, 0, sizeof(perThread));
    }

    BackendX64()
        : Backend{nullptr}
    {
        memset(perThread, 0, sizeof(perThread));
    }

    bool                        createRuntime(const BuildParameters& buildParameters);
    JobResult                   prepareOutput(int stage, const BuildParameters& buildParameters, Job* ownerJob) override;
    bool                        generateOutput(const BuildParameters& backendParameters) override;
    BackendFunctionBodyJobBase* newFunctionJob() override;

    uint32_t getOrCreateLabel(X64Gen& pp, uint32_t ip);
    uint16_t computeUnwindPushRDI(uint32_t offsetSubRSP);
    void     computeUnwindStack(uint32_t sizeStack, uint32_t offsetSubRSP, VectorNative<uint16_t>& unwind);
    void     emitOverflowSigned(X64Gen& pp, AstNode* node, const char* msg);
    void     emitOverflowUnsigned(X64Gen& pp, AstNode* node, const char* msg);
    void     emitShiftArithmetic(X64Gen& pp, ByteCodeInstruction* ip, uint8_t numBits);
    void     emitShiftLogical(X64Gen& pp, ByteCodeInstruction* ip, uint8_t numBits, uint8_t op);
    void     emitShiftEqArithmetic(X64Gen& pp, ByteCodeInstruction* ip, uint8_t numBits);
    void     emitShiftEqLogical(X64Gen& pp, ByteCodeInstruction* ip, uint8_t numBits, uint8_t op);
    void     emitInternalPanic(X64Gen& pp, AstNode* node, const char* msg);
    bool     emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc);
    void     emitBinOpFloat32(X64Gen& pp, ByteCodeInstruction* ip, X64Op op);
    void     emitBinOpFloat32AtReg(X64Gen& pp, ByteCodeInstruction* ip, X64Op op);
    void     emitBinOpFloat64(X64Gen& pp, ByteCodeInstruction* ip, X64Op op);
    void     emitBinOpFloat64AtReg(X64Gen& pp, ByteCodeInstruction* ip, X64Op op);
    void     emitBinOpInt16(X64Gen& pp, ByteCodeInstruction* ip, X64Op op);
    void     emitBinOpInt16AtReg(X64Gen& pp, ByteCodeInstruction* ip, X64Op op);
    void     emitBinOpInt32(X64Gen& pp, ByteCodeInstruction* ip, X64Op op);
    void     emitBinOpInt32AtReg(X64Gen& pp, ByteCodeInstruction* ip, X64Op op);
    void     emitBinOpInt64(X64Gen& pp, ByteCodeInstruction* ip, X64Op op);
    void     emitBinOpInt64AtReg(X64Gen& pp, ByteCodeInstruction* ip, X64Op op);
    void     emitBinOpInt8(X64Gen& pp, ByteCodeInstruction* ip, X64Op op);
    void     emitBinOpInt8AtReg(X64Gen& pp, ByteCodeInstruction* ip, X64Op op);
    void     emitBinOpIntDivAtReg(X64Gen& pp, ByteCodeInstruction* ip, bool isSigned, uint32_t bits, bool modulo = false);

    bool emitXData(const BuildParameters& buildParameters);
    bool emitPData(const BuildParameters& buildParameters);
    bool emitDirectives(const BuildParameters& buildParameters);
    bool emitSymbolTable(const BuildParameters& buildParameters);
    bool emitStringTable(const BuildParameters& buildParameters);
    bool emitRelocationTable(Concat& concat, CoffRelocationTable& cofftable, uint32_t* sectionFlags, uint16_t* count);
    bool emitHeader(const BuildParameters& buildParameters);

    DbgTypeIndex dbgEmitTypeSlice(X64Gen& pp, TypeInfo* typeInfo, TypeInfo* pointedType);
    void         dbgEmitEmbeddedValue(Concat& concat, TypeInfo* valueType, ComputedValue& val);
    void         dbgStartRecord(X64Gen& pp, Concat& concat, uint16_t what);
    void         dbgEndRecord(X64Gen& pp, Concat& concat, bool align = true);
    void         dbgEmitSecRel(X64Gen& pp, Concat& concat, uint32_t symbolIndex, uint32_t segIndex, uint32_t offset = 0);
    void         dbgEmitTruncatedString(Concat& concat, const Utf8& str);
    DbgTypeIndex dbgGetSimpleType(TypeInfo* typeInfo);
    DbgTypeIndex dbgGetOrCreatePointerToType(X64Gen& pp, TypeInfo* typeInfo, bool asRef);
    DbgTypeIndex dbgGetOrCreatePointerPointerToType(X64Gen& pp, TypeInfo* typeInfo);
    void         dbgRecordFields(X64Gen& pp, DbgTypeRecord* tr, TypeInfoStruct* typeStruct, uint32_t baseOffset);
    DbgTypeIndex dbgGetOrCreateType(X64Gen& pp, TypeInfo* typeInfo, bool forceUnRef = false);
    void         dbgAddTypeRecord(X64Gen& pp, DbgTypeRecord* tr);
    Utf8         dbgGetScopedName(AstNode* node);
    void         dbgSetLocation(CoffFunction* coffFct, ByteCode* bc, ByteCodeInstruction* ip, uint32_t byteOffset);
    void         dbgEmitCompilerFlagsDebugS(Concat& concat);
    void         dbgEmitConstant(X64Gen& pp, Concat& concat, AstNode* node, const Utf8& name);
    void         dbgEmitGlobalDebugS(X64Gen& pp, Concat& concat, VectorNative<AstNode*>& gVars, uint32_t segSymIndex);
    bool         dbgEmitDataDebugT(const BuildParameters& buildParameters);
    bool         dbgEmitFctDebugS(const BuildParameters& buildParameters);
    bool         dbgEmitScope(X64Gen& pp, Concat& concat, CoffFunction& f, Scope* scope);
    bool         emitDebug(const BuildParameters& buildParameters);
    void         emitByteCodeCall(X64Gen& pp, TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams);
    void         emitByteCodeCallParameters(X64Gen& pp, TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams);

    bool saveObjFile(const BuildParameters& buildParameters);

    bool buildRelocSegment(const BuildParameters& buildParameters, DataSegment* dataSegment, CoffRelocationTable& relocTable, SegmentKind me);

    bool emitGetTypeTable(const BuildParameters& buildParameters);
    bool emitGlobalPreMain(const BuildParameters& buildParameters);
    bool emitGlobalInit(const BuildParameters& buildParameters);
    bool emitGlobalDrop(const BuildParameters& buildParameters);
    bool emitOS(const BuildParameters& buildParameters);
    bool emitMain(const BuildParameters& buildParameters);
    void emitSymbolRelocation(X64Gen& pp, const Utf8& name);

    uint32_t getParamStackOffset(TypeInfoFuncAttr* typeFunc, int paramIdx, int offsetS4, int sizeStack);
    void     emitGetParam(X64Gen& pp, TypeInfoFuncAttr* typeFunc, int reg, int paramIdx, int sizeOf, int storeS4, int sizeStack, uint64_t toAdd = 0, int derefSize = 0);
    void     emitCall(X64Gen& pp, TypeInfoFuncAttr* typeFunc, const Utf8& funcName, const VectorNative<X64PushParam>& pushParams, uint32_t offsetRT, bool localCall);
    void     emitCall(X64Gen& pp, TypeInfoFuncAttr* typeFunc, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT, bool localCall);
    void     emitInternalCall(X64Gen& pp, Module* moduleToGen, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT = UINT32_MAX);
    void     emitInternalCallExt(X64Gen& pp, Module* moduleToGen, const Utf8& funcName, const VectorNative<X64PushParam>& pushParams, uint32_t offsetRT = UINT32_MAX, TypeInfoFuncAttr* typeFunc = nullptr);
    void     emitCall(X64Gen& pp, const Utf8& funcName);

    CoffFunction* registerFunction(X64Gen& pp, AstNode* node, uint32_t symbolIndex);
    void          registerFunction(CoffFunction* fct, uint32_t startAddress, uint32_t endAddress, uint32_t sizeProlog, VectorNative<uint16_t>& unwind);

    X64Gen* perThread[BackendCompileType::Count][MAX_PRECOMPILE_BUFFERS];
};