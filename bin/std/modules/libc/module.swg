// Wrapper for LibC library.

#dependencies
{
    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion = 0
        cfg.moduleRevision = 1
        cfg.moduleBuildNum = 0
        cfg.moduleNamespace = "Libc"
        cfg.backendKind = .Export
    }
}