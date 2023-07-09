#dependencies
{
    #run
    {
        var itf = @compiler()
        var cfg = itf.getBuildCfg()
        cfg.moduleVersion  = 1
        cfg.moduleRevision = 0
        cfg.moduleBuildNum = 0
        cfg.backendKind    = .DynamicLib
        cfg.warnDefaultDisabled = true
    }
}