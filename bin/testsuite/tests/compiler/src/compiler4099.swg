//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    if @isbytecode()
    {
        var cfg: *Swag.BuildCfg
        cfg = @compiler().getBuildCfg()
        //@print(cfg.debugAllocator)
        var cfg1 = @compiler().getBuildCfg()
        @assert(cfg == cfg1)
    }
}