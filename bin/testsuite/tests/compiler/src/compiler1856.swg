#message(Swag.CompilerMsgMask.PassBeforeOutput)
{
    let itf = @compiler()
    let msg = itf.getMessage()
    @assert(msg.moduleName == "test_compiler")

    let cfg = itf.getBuildCfg()
}
