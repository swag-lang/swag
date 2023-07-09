// Wrapper for LibC library.

#dependencies
{
    #run
    {
        let itf = @compiler()
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "Libc"
        cfg.backendKind = .Export
        cfg.repoPath = "https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc"
    }
}