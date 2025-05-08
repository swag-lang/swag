// Wrapper for Microsoft XAudio2 library.

#dependencies
{
    #import("win32")

    #run
    {
        let itf = @compiler
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion   = #swagversion
        cfg.moduleRevision  = #swagrevision
        cfg.moduleBuildNum  = #swagbuildnum
        cfg.moduleNamespace = "XAudio2"
        cfg.backendKind     = .Export
        cfg.repoPath        = "https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2"

        cfg.genDoc.kind            = .Api
        cfg.genDoc.outputExtension = ".php"
        cfg.genDoc.css             = "css/style.css"
        cfg.genDoc.startHead       = "<?php include('common/start-head.php'); ?>"
        cfg.genDoc.endHead         = "<?php include('common/end-head.php'); ?>"
        cfg.genDoc.startBody       = "<?php include('common/start-body.php'); ?>"
    }
}
