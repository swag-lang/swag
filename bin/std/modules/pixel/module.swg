#dependencies
{
    #import "core"
    #import "ogl"
    #import "freetype"

    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "Pixel"
        cfg.backendKind = .DynamicLib
    }
}