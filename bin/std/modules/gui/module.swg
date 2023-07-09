#global using Core

#dependencies
{
    #import "pixel"

    #run
    {
        let itf = @compiler()
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "Gui"
        cfg.backendKind = .DynamicLib
        cfg.repoPath = "https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui"
    }
}