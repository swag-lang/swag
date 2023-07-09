// Wrappers for Microsoft Windows Sdk Libraries.
// 10.0.19041.0

#dependencies
{
    #run
    {
        let itf = @compiler()
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "Win32"
        cfg.backendKind = .Export
        cfg.repoPath = "https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32"
    }
}