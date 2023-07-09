#dependencies
{
    #import "core"
    #if #os == Swag.TargetOs.Windows #import "gdi32"

    #run
    {
        var itf = @compiler()
        var cfg = itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "Ogl"
        cfg.backendKind = .DynamicLib
        cfg.repoPath = "https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl"
    }
}
