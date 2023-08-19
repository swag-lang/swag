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
        /*
        cfg.docPageHeader = """
                            <?php include('html-start.php'); ?>
                            <?php include('header.php'); ?>
                            """
        cfg.docPageFooter = """
                            <?php include('html-end.php'); ?>
                            """
        */
    }
}