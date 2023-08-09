/*
Toto la foule
*/

#dependencies
{
    #if #os == Swag.TargetOs.Windows
    {
        #import "core"
        #import "xaudio2"
    }

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