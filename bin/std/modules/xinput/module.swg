// Wrapper for Microsoft DirectX Input.

#dependencies
{
    #import "win32"

    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion  = 0
        cfg.moduleRevision = 1
        cfg.moduleBuildNum = 0
        cfg.moduleNamespace = "XInput"
        cfg.backendKind = .Export
    }
}
