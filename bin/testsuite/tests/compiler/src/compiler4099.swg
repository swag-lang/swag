//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#test
{
    if @isbytecode
    {
        var cfg: *Swag.BuildCfg
        cfg = @compiler().getBuildCfg()
        //@print(cfg.debugAllocator)
        let cfg1 = @compiler().getBuildCfg()
        @assert(cfg == cfg1)
    }
}