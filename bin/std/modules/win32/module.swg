// Wrappers for Microsoft Windows Sdk Libraries.
// 10.0.19041.0

#dependencies
{
    #run
    {
        let itf = @compiler()
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "Win32"
        cfg.backendKind = .Export
        cfg.repoPath = "https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32"

        cfg.docKind = .Api
        cfg.docOutputExtension = ".php"
        cfg.docCss = "css/style.css"
        cfg.docStartHead = @"<?php include('common/start-head.php'); ?>"@
        cfg.docEndHead   = @"<?php include('common/end-head.php'); ?>"@
        cfg.docStartBody = @"<?php include('common/start-body.php'); ?>"@
    }
}