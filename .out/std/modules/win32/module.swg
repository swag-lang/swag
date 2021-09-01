// Wrapper for Microsoft Kernel32 and User32.

#dependencies
{
    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion = 0
        cfg.moduleRevision = 0
        cfg.moduleBuildNum = 0
        cfg.moduleNamespace = "Win32"
        cfg.backendKind = .Export
    }
}