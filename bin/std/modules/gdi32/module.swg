// Wrapper for Microsoft Gdi32 library.

#dependencies
{
    #import "win32"

    #run
    {
        let itf = @compiler()
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "Gdi32"
        cfg.backendKind = .Export
        cfg.docKind = .Api
        cfg.repoPath = "https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32"
    }
}