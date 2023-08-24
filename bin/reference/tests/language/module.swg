#dependencies
{
    #run
    {
        let itf = @compiler()
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion  = 1
        cfg.moduleRevision = 0
        cfg.moduleBuildNum = 0
        cfg.backendKind    = .Executable
        cfg.warnDefaultDisabled = true
        cfg.warnAsWarning = "Wrn0009"

        cfg.docTitleContent = "Swag Language Reference"
        cfg.docCss = "css/style.css"
        cfg.docIcon = "favicon.ico"
        cfg.docKind = .Examples
        cfg.docOutputName = "language"
        cfg.docOutputExtension = ".php"

        cfg.docStartHead = "<?php include('common/start-head.php'); ?>"
        cfg.docEndHead   = "<?php include('common/end-head.php'); ?>"
        cfg.docStartBody = "<?php include('common/start-body.php'); ?>"
    }
}