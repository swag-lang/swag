// Wrapper for Microsoft Gdi32 library.

#dependencies
{
    #import "win32"

    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion = 0
        cfg.moduleRevision = 1
        cfg.moduleBuildNum = 0
        cfg.moduleNamespace = "Gdi32"
        cfg.backendKind = .Export
    }
}