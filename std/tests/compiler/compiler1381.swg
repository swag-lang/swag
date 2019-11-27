struct CompilerTarget
{
    // Bytecode
    bytecodeBoundCheck: bool    = true
    bytecodeAnyCastCheck: bool  = true
    byteCodeMaxRecurse: u32     = 1024
    byteCodeStackSize: u32      = 16 * 1024

    // Backend common
    backendDebugInformations: bool  = false
    backendOptimizeLevel: u32       = 0
    
    // Backend specific
    backendC: CompilerParamsBackendC
}

struct CompilerParamsBackendC
{
    writeSourceCode: bool           = false
    writeByteCodeInstruction: bool  = false
    maxApplicationArguments: u32    = 64
}

#test
{
    ptr := cast(const *swag.TypeInfoStruct) @typeof(CompilerTarget)
    @assert(ptr.fields[0].offset == 0) // bytecodeBoundCheck
    @assert(ptr.fields[1].offset == 1) // bytecodeAnyCastCheck
    @assert(ptr.fields[2].offset == 4) // byteCodeMaxRecurse
    @assert(ptr.fields[3].offset == 8) // byteCodeStackSize
    @assert(ptr.fields[4].offset == 12) // backendDebugInformations
    @assert(ptr.fields[5].offset == 16) // backendOptimizeLevel
    @assert(ptr.fields[6].offset == 20) // CompilerParamsBackendC
}