//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    if @isbytecode()
    {
        var cfg = @compiler().getBuildCfg().safetyGuards
        @assert(@sizeof(cfg) == 2)
    }
}