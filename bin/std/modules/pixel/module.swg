#dependencies
{
    #import "core"
    #import "ogl"
    #import "freetype"

    #run
    {
        let itf = @compiler()
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "Pixel"
        cfg.backendKind = .DynamicLib
        cfg.repoPath = "https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel"
    }
}