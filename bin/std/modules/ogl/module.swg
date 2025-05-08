#dependencies
{
    #import("core")
    #if #os == Swag.TargetOs.Windows
    {
        #import("gdi32")
    }

    #run
    {
        let itf = @compiler
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion   = #swagversion
        cfg.moduleRevision  = #swagrevision
        cfg.moduleBuildNum  = #swagbuildnum
        cfg.moduleNamespace = "Ogl"
        cfg.backendKind     = .Library
        cfg.repoPath        = "https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl"

        cfg.genDoc.kind            = .Api
        cfg.genDoc.outputExtension = ".php"
        cfg.genDoc.css             = "css/style.css"
        cfg.genDoc.startHead       = "<?php include('common/start-head.php'); ?>"
        cfg.genDoc.endHead         = "<?php include('common/end-head.php'); ?>"
        cfg.genDoc.startBody       = "<?php include('common/start-body.php'); ?>"
    }
}
