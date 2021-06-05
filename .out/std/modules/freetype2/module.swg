#dependencies
{
    #import "win32"

    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion = 2
        cfg.moduleRevision = 10
        cfg.moduleBuildNum = 4
        cfg.moduleNamespace = "FreeType2"
        cfg.backendKind = .Export
    }
}