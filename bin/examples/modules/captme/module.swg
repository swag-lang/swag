#global using Core, Pixel, Gui

#dependencies
{
    #import("gui", location: "swag@std")

    #run
    {
        let itf = @compiler
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion  = 0
        cfg.moduleRevision = 0
        cfg.moduleBuildNum = 0
        cfg.backendKind    = .Executable
        cfg.backendSubKind = .Default
    }
}

#message(Swag.CompilerMsgMask.PassBeforeOutput)
{
    let itf = @compiler
    let cfg = itf.getBuildCfg()
    cfg.resIcoFileName = Path.getDirectoryName(#file) ++ "/src/datas/appicon.ico"
}
