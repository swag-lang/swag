#dependencies
{
    #import "core"
    #import "image"
    #import "ogl"
    #import "mthx"

    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion = 0
        cfg.moduleRevision = 0
        cfg.moduleBuildNum = 0
        cfg.moduleNamespace = "Paint"
        cfg.backendKind = .DynamicLib
    }
}