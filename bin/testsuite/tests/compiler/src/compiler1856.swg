#message(Swag.CompilerMsgMask.PassBeforeOutput)
{
    itf := @compiler()
    msg := itf.getMessage()
    @assert(msg.moduleName == "test_compiler")

    cfg := itf.getBuildCfg()
    //cfg.backendLLVM.outputIR = true
}