/*
    :LICENCE
    The freetype module is a wrapper for the great Freetype 2 library.
    See LICENCE.md for the corresponding licence (FTL).
*/

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
        cfg.moduleNamespace = "FreeType"
        cfg.backendKind = .Export
        cfg.repoPath = "https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype"

        cfg.docKind = .Api
        cfg.docOutputExtension = ".php"
        cfg.docCss = "css/style.css"
        cfg.docStartHead = "<?php include('common/start-head.php'); ?>"
        cfg.docEndHead   = "<?php include('common/end-head.php'); ?>"
        cfg.docStartBody = "<?php include('common/start-body.php'); ?>"
    }
}