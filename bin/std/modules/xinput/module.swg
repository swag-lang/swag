// Wrapper for Microsoft DirectX Input.

#dependencies
{
    #import "win32"

    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion  = 10
        cfg.moduleRevision = 0
        cfg.moduleBuildNum = 19041
        cfg.moduleNamespace = "XInput"
        cfg.backendKind = .Export
    }
}
