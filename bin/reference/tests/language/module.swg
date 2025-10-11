#dependencies
{
    #run
    {
        let itf = @compiler
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion       = 1
        cfg.moduleRevision      = 0
        cfg.moduleBuildNum      = 0
        cfg.backendKind         = .Executable
        cfg.warnDefaultDisabled = true
        cfg.warnAsWarning       = "Wrn0003"

        cfg.genDoc.kind              = .Examples
        cfg.genDoc.titleContent      = "Swag Language Reference"
        cfg.genDoc.css               = "css/style.css"
        cfg.genDoc.icon              = "favicon.ico"
        cfg.genDoc.outputName        = "language"
        cfg.genDoc.outputExtension   = ".php"
        cfg.genDoc.startHead         = "<?php include('common/start-head.php'); ?>"
        cfg.genDoc.endHead           = "<?php include('common/end-head.php'); ?>"
        cfg.genDoc.startBody         = "<?php include('common/start-body.php'); ?>"
        cfg.genDoc.quoteTitleExample = "Result"
    }
}
