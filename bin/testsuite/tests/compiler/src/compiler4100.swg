//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    if @isbytecode()
    {
        cfg := @compiler().getBuildCfg().safetyGuards
        @assert(@typeof(cfg) == u64)
    }
}