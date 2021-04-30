#dependencies
{
    #import "core" location="swag@std"
    #import "image" location="swag@std"
    #import "ogl" location="swag@std"

    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion  = 0
        cfg.moduleRevision = 0
        cfg.moduleBuildNum = 0
        cfg.backendKind    = .Executable
    }
}