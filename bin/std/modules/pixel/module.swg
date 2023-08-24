#dependencies
{
    #import "core"
    #import "ogl"
    #import "freetype"

    #run
    {
        let itf = @compiler()
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "Pixel"
        cfg.backendKind = .DynamicLib
        cfg.repoPath = "https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel"

        cfg.docKind = .Api
        cfg.docOutputExtension = ".php"
        cfg.docCss = "css/style.css"
        cfg.docStartHead = "<?php include('common/start-head.php'); ?>"
        cfg.docEndHead   = "<?php include('common/end-head.php'); ?>"
        cfg.docStartBody = "<?php include('common/start-body.php'); ?>"
    }
}