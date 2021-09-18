#dependencies
{
    #import "pixel"

    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "Gui"
        cfg.backendKind = .DynamicLib
    }
}