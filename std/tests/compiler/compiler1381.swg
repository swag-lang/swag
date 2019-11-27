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