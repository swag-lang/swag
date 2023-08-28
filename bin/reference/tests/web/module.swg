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

        cfg.docKind = .Pages
        cfg.docOutputExtension = ".php"

        cfg.docTitleContent = "Swag"
        cfg.docCss = "css/style.css"
        cfg.docIcon = "favicon.ico"
        cfg.docStartHead = "<?php include('common/start-head.php'); ?>"
        cfg.docStartBody = "<?php include('common/start-body.php'); ?>"

        cfg.docMorePages = "../../../examples/scripts/flappy.swgs"
    }
}