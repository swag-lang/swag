#dependencies
{
    #if #os == "windows"
    {
        #import "win32"
        #import "xinput"
    }

    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion = 0
        cfg.moduleRevision = 0
        cfg.moduleBuildNum = 0
        cfg.backendKind = .DynamicLib
    }
}