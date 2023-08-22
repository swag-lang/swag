#dependencies
{
    #import "core"
    #if #os == Swag.TargetOs.Windows
    {
        #import "gdi32"
    }

    #run
    {
        let itf = @compiler()
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "Ogl"
        cfg.backendKind = .DynamicLib
        cfg.repoPath = "https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl"

        cfg.docKind = .Api
        cfg.docOutputExtension = ".php"
        cfg.docCss = "css/style.css"
        cfg.docStartHead = @"<?php include('common/start-head.php'); ?>"@
        cfg.docEndHead   = @"<?php include('common/end-head.php'); ?>"@
        cfg.docStartBody = @"<?php include('common/start-body.php'); ?>"@
    }
}
