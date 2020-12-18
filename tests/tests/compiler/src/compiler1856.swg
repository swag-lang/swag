#compilerpass

#compiler(swag.CompilerMsgMask.PassBeforeOutput)
{
    itf := @compiler()
    msg := itf.getMessage()
    @assert(msg.moduleName == "tests_compiler")

    cfg := itf.getBuildCfg()
    //cfg.backendLLVM.outputIR = true
}