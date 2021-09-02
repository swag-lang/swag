#dependencies
{
    #import "core"
    #if #os == "windows" #import "gdi32"

    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion = 0
        cfg.moduleRevision = 0
        cfg.moduleBuildNum = 0
        cfg.moduleNamespace = "Ogl"
        cfg.backendKind = .DynamicLib
    }
}
