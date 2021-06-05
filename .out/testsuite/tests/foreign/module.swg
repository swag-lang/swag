#dependencies
{
    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion  = 1
        cfg.moduleRevision = 0
        cfg.moduleBuildNum = 0
        cfg.backendKind    = .DynamicLib
    }
}