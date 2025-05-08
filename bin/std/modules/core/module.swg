// The 'Std.core' module contains basic types and algorithms.
// It is the base of all other modules of the standard 'Std' workspace.
//
// You will find collections like [[Array]] or [[HashTable]], a dynamic [[String]] and so on.
#dependencies
{
    #if #os == Swag.TargetOs.Windows
    {
        #import("win32")
        #import("xinput")
    }

    #run
    {
        let itf = @compiler
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion   = #swagversion
        cfg.moduleRevision  = #swagrevision
        cfg.moduleBuildNum  = #swagbuildnum
        cfg.moduleNamespace = "Core"
        cfg.backendKind     = .Library
        cfg.repoPath        = "https://github.com/swag-lang/swag/blob/master/bin/std/modules/core"

        cfg.genDoc.kind            = .Api
        cfg.genDoc.icon            = "favicon.ico"
        cfg.genDoc.titleContent    = "Swag Std.Core"
        cfg.genDoc.outputExtension = ".php"
        cfg.genDoc.css             = "css/style.css"
        cfg.genDoc.startHead       = "<?php include('common/start-head.php'); ?>"
        cfg.genDoc.endHead         = "<?php include('common/end-head.php'); ?>"
        cfg.genDoc.startBody       = "<?php include('common/start-body.php'); ?>"
    }
}
