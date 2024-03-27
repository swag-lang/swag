//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    if @isbytecode()
    {
        let cfg = @compiler().getBuildCfg().safetyGuards
        @assert(@sizeof(cfg) == 2)
    }
}
