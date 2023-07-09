#global using Core, Pixel, Gui

#dependencies
{
    #import "gui" location="swag@std"

    #run
    {
        var itf = @compiler()
        var cfg = itf.getBuildCfg()
        cfg.moduleVersion  = 0
        cfg.moduleRevision = 0
        cfg.moduleBuildNum = 0
        cfg.backendKind    = .Executable
        cfg.backendSubKind = .Default
    }
}