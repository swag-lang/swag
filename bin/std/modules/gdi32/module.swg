// Wrapper for Microsoft Gdi32 library.

#dependencies
{
    #import "win32"

    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "Gdi32"
        cfg.backendKind = .Export
    }
}