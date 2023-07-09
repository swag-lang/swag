#message(Swag.CompilerMsgMask.PassBeforeOutput)
{
    var itf = @compiler()
    var msg = itf.getMessage()
    @assert(msg.moduleName == "test_compiler")

    var cfg = itf.getBuildCfg()
    //cfg.backendLLVM.outputIR = true
}