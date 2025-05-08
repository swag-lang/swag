#dependencies
{
    #run
    {
        let itf = @compiler
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion  = 1
        cfg.moduleRevision = 0
        cfg.moduleBuildNum = 0
        cfg.backendKind    = .Executable

        cfg.genDoc.kind            = .Pages
        cfg.genDoc.outputExtension = ".php"
        cfg.genDoc.titleContent    = "Swag"
        cfg.genDoc.css             = "css/style.css"
        cfg.genDoc.icon            = "favicon.ico"
        cfg.genDoc.startHead       = "<?php include('common/start-head.php'); ?>"
        cfg.genDoc.startBody       = "<?php include('common/start-body.php'); ?>"
        cfg.genDoc.morePages       = "../../../examples/scripts/flappy.swgs"
    }
}
