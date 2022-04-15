// Wrapper for Microsoft Kernel32 and User32.
// 10.0.19041.0

#dependencies
{
    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "Win32"
        cfg.backendKind = .Export
    }
}