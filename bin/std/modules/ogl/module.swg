#dependencies
{
    #import "core"
    #if #os == "windows" #import "gdi32"

    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion = 10
        cfg.moduleRevision = 0
        cfg.moduleBuildNum = 19041
        cfg.moduleNamespace = "Ogl"
        cfg.backendKind = .DynamicLib
    }
}
