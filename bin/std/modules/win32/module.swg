// Wrapper for Microsoft Kernel32 and User32.

#dependencies
{
    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion = 10
        cfg.moduleRevision = 0
        cfg.moduleBuildNum = 19041
        cfg.moduleNamespace = "Win32"
        cfg.backendKind = .Export
    }
}