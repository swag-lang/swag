// Wrapper for Microsoft Gdi32 library.

#dependencies
{
    #import "win32"

    #run
    {
        let itf = @compiler()
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "Gdi32"
        cfg.backendKind = .Export
        cfg.repoPath = "https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32"

        cfg.docKind = .Api
        cfg.docOutputExtension = ".php"
        cfg.docCss = "css/style.css"
        cfg.docStartHead = "<?php include('common/start-head.php'); ?>"
        cfg.docEndHead   = "<?php include('common/end-head.php'); ?>"
        cfg.docStartBody = "<?php include('common/start-body.php'); ?>"
    }
}