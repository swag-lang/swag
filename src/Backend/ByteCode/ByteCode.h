#pragma once
#include "Backend/ByteCode/ByteCodeInstruction.h"
#include "Backend/ByteCode/ByteCodeOp.h"
#include "Backend/ByteCode/Profiler/Profiler.h"
#include "Backend/ByteCode/Register.h"
#include "Report/Log.h"
#include "Threading/Mutex.h"
#include "Threading/RaceCondition.h"

enum class LogColor;
struct AstNode;
struct SourceFile;
struct TypeInfoFuncAttr;
struct ByteCode;
struct SourceLocation;

using ByteCodePrintPrintFlags                  = Flags<uint32_t>;
constexpr ByteCodePrintPrintFlags BCPF_DEFAULT = 0x00000000;

struct ByteCodePrintOptions
{
    ByteCodeInstruction*    curIp           = nullptr;
    bool                    printSourceCode = true;
    ByteCodePrintPrintFlags flags           = BCPF_DEFAULT;
    LogColor                rankColor       = LogColor::Undefined;
    LogColor                nameColor       = LogColor::Undefined;
    LogColor                argsColor       = LogColor::Undefined;
    LogColor                flagsColor      = LogColor::Undefined;
    LogColor                prettyColor     = LogColor::Undefined;
};

struct ByteCode
{
    struct Location
    {
        SourceFile*     file     = nullptr;
        SourceLocation* location = nullptr;
        AstNode*        node     = nullptr;
    };

    struct PrintInstructionLine
    {
        Utf8 bkp;
        Utf8 rank;
        Utf8 name;
        Utf8 args;
        Utf8 flags;
        Utf8 pretty;
#ifdef SWAG_DEV_MODE
        Utf8 devMode;
#endif
    };

    void release();

    static bool        hasOpFlag(ByteCodeOp op, OpFlags fl) { return g_ByteCodeOpDesc[static_cast<int>(op)].flags.has(fl); }
    static OpFlags     opFlags(ByteCodeOp op) { return g_ByteCodeOpDesc[static_cast<int>(op)].flags; }
    static const char* opName(ByteCodeOp op) { return g_ByteCodeOpDesc[static_cast<int>(op)].name; }

    static bool haveSameRead(const ByteCodeInstruction* ip0, const ByteCodeInstruction* ip1);

    static void*    doByteCodeLambda(void* ptr);
    static void*    undoByteCodeLambda(void* ptr);
    static bool     isByteCodeLambda(void* ptr);
    static Location getLocation(const ByteCode* bc, const ByteCodeInstruction* ip, bool wantInline = false);
    static uint32_t getSetZeroAtPointerSize(const ByteCodeInstruction* ip, uint32_t& offset);
    static uint32_t getSetZeroStackSize(const ByteCodeInstruction* ip, uint32_t& offset);

    static void printSourceCode(const ByteCodePrintOptions& options, const ByteCode* bc, const ByteCodeInstruction* ip, uint32_t* lastLine, SourceFile** lastFile, AstNode** lastInline);
    static Utf8 getPrettyInstruction(const ByteCodeInstruction* ip);
    static Utf8 getInstructionReg(const char* name, const Register& reg, bool regW, bool regR, bool regImm);
    static void fillPrintInstruction(const ByteCodePrintOptions& options, const ByteCode* bc, const ByteCodeInstruction* ip, PrintInstructionLine& line);
    static void printInstruction(const ByteCodePrintOptions& options, const ByteCodeInstruction* ip, const PrintInstructionLine& line);
    static void alignPrintInstructions(const ByteCodePrintOptions& options, Vector<PrintInstructionLine>& lines, bool defaultLen = false);

    void printInstruction(const ByteCodePrintOptions& options, const ByteCodeInstruction* ip) const;
    void print(const ByteCodePrintOptions& options, uint32_t start, uint32_t count) const;
    void print(const ByteCodePrintOptions& options) const;
    void printName() const;
    Utf8 getPrintRefName() const;
    Utf8 getPrintName() const;
    Utf8 getPrintFileName() const;

    Utf8              getCallNameFromDecl();
    Utf8              getCallName();
    TypeInfoFuncAttr* getCallType() const;
    bool              canEmit() const;
    void              markLabels() const;
    bool              isDoingNothing() const;
    void              makeRoomForInstructions(uint32_t room = 1);
    static void       swapInstructions(ByteCodeInstruction* ip0, ByteCodeInstruction* ip1);

    static bool areSame(const ByteCodeInstruction* start0, const ByteCodeInstruction* end0, const ByteCodeInstruction* start1, const ByteCodeInstruction* end1, bool specialJump, bool specialCall);
    uint32_t    computeCrc(const ByteCodeInstruction* ip, uint32_t oldCrc, bool specialJump, bool specialCall) const;

    VectorNative<uint32_t>                 availableRegistersRC;
    VectorNative<std::pair<void*, size_t>> autoFree;

    Mutex                  mutexCallName;
    Utf8                   name;
    Utf8                   callName;
    VectorNative<AstNode*> localVars;
    VectorNative<AstNode*> dependentCalls;
    SetUtf8                hasForeignFunctionCallsModules;

#ifdef SWAG_STATS
    MapUtf8<FFIStat> ffiProfile;
    uint64_t         profileStart     = 0;
    uint64_t         profileCumTime   = 0;
    uint64_t         profileFFI       = 0;
    uint32_t         profileCallCount = 0;
    double           profilePerCall   = 0;
    Set<ByteCode*>   profileChildren;
#endif

    ByteCodeInstruction* out          = nullptr;
    SourceFile*          sourceFile   = nullptr;
    TypeInfoFuncAttr*    typeInfoFunc = nullptr;
    AstNode*             node         = nullptr;

    uint32_t stackSize             = 0;
    uint32_t dynStackSize          = 0;
    uint32_t numInstructions       = 0;
    uint32_t maxInstructions       = 0;
    uint32_t maxCallParams         = 0;
    uint32_t maxSpVaargs           = 0;
    uint32_t maxReservedRegisterRC = 0;
    uint32_t numJumps              = 0;
    uint32_t numDebugNops          = 0;
    uint32_t staticRegs            = 0;
    uint32_t registerGetContext    = UINT32_MAX;
    uint32_t registerStoreRR       = UINT32_MAX;
    uint32_t emitSPSize            = 0;

    bool isCompilerGenerated     = false;
    bool isAddedToList           = false;
    bool isDirtyRegistersRC      = false;
    bool isUsed                  = false;
    bool isInDataSegment         = false;
    bool hasForeignFunctionCalls = false;
    bool sanDone                 = false;

    std::atomic<bool> isEmpty;

    SWAG_RACE_CONDITION_INSTANCE(raceCond);
};
