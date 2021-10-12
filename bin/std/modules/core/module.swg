#dependencies
{
    #if #os == Swag.TargetOs.Windows
    {
        #import "win32"
        #import "xinput"
    }

    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "Core"
        cfg.backendKind = .DynamicLib
    }
}