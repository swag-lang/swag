// 'Std.Audio' is a module to decode and play sound files.
// Under windows, it is based on the 'xaudio2' library.

#dependencies
{
    #import "core"
    #if #os == Swag.TargetOs.Windows
        #import "xaudio2"

    #run
    {
        let itf = @compiler()
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "Audio"
        cfg.backendKind = .DynamicLib
        cfg.repoPath = "https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio"
    }
}