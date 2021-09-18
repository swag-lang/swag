// Wrapper for LibC library.

#dependencies
{
    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "Libc"
        cfg.backendKind = .Export
    }
}