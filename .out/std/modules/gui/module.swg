#dependencies
{
    #if #os == "windows"
    {
        #import "pixel"
    }

    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion = 0
        cfg.moduleRevision = 0
        cfg.moduleBuildNum = 0
        cfg.moduleNamespace = "Gui"
        cfg.backendKind = .DynamicLib
    }
}