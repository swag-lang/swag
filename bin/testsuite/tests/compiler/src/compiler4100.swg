//#[Swag.PrintBc]
#[Swag.Optimize(.All, false)]
#test
{
    if @bytecode
    {
        let cfg = @compiler.getBuildCfg().safetyGuards
        @assert(#sizeof(cfg) == 2)
    }
}
