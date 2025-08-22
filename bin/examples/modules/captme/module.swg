#global using Core, Pixel, Gui

#dependencies
{
    #import("gui", location: "swag@std")

    #run
    {
        let itf = @compiler
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion  = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.backendKind    = .Executable
        cfg.backendSubKind = .Default
        cfg.resAppName     = "Captme"
    }
}

#message(Swag.CompilerMsgMask.PassBeforeOutput)
{
    let itf = @compiler
    let cfg = itf.getBuildCfg()
    cfg.resAppIcoFileName = Path.getDirectoryName(#file) ++ "/src/datas/appicon.ico"
}
