#dependencies
{
    #import "gui" location="swag@std"

    #run
    {
        let itf = @compiler()
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion  = 0
        cfg.moduleRevision = 0
        cfg.moduleBuildNum = 0
        cfg.embbedImports  = true
        cfg.backendKind    = .Executable
        cfg.backendSubKind = .Default
    }
}