// Wrapper for Microsoft DirectX Input.

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
        cfg.moduleNamespace = "XInput"
        cfg.backendKind = .Export
    }
}
