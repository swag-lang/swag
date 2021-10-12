#dependencies
{
    #import "core"
    #if #os == Swag.TargetOs.Windows #import "gdi32"

    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "Ogl"
        cfg.backendKind = .DynamicLib
    }
}
