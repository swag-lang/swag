#dependencies
{
    #import "core"
    #import "libc"

    #run
    {
        var itf = @compiler()
        var cfg = itf.getBuildCfg()
        cfg.moduleVersion  = 0
        cfg.moduleRevision = 0
        cfg.moduleBuildNum = 0
        cfg.backendKind    = .Executable
    }
}