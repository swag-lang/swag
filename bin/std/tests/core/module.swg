#dependencies
{
    #import "core"

    #run
    {
        let itf = @compiler()
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion  = 0
        cfg.moduleRevision = 0
        cfg.moduleBuildNum = 0
        cfg.backendKind    = .Executable
        cfg.warnDefaultDisabled = true
        cfg.warnAsWarning = "Wrn0009"
    }
}
