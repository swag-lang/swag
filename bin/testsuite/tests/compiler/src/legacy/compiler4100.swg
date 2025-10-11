//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#test
{
    if @bytecode
    {
        let cfg = @compiler.getBuildCfg().safetyGuards
        @assert(#sizeof(cfg) == 2)
    }
}
