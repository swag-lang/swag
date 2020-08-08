#compiler(swag.CompilerMsgKindMask.PassBeforeOutput)
{
    itf := @compiler()
    msg := itf.getMessage()
    @assert(msg.moduleName == "tests.compiler")

    cfg := itf.getBuildCfg()
    //cfg.backendLLVM.outputIR = true
}