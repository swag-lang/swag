//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    if @isbytecode()
    {
        cfg := @compiler().getBuildCfg().safetyGuards
        @assert(@sizeof(cfg) == 2)
    }
}