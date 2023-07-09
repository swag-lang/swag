// The 'Std.core' module contains basic types and algorithms.
// It is the base of all other modules of the standard 'Std' workspace.
//
// You will find collections like [Array] or [HashTable], a dynamic [String] and so on.
#dependencies
{
    #if #os == Swag.TargetOs.Windows
    {
        #import "win32"
        #import "xinput"
    }

    #run
    {
        var itf = @compiler()
        var cfg = itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "Core"
        cfg.backendKind = .DynamicLib
        cfg.repoPath = "https://github.com/swag-lang/swag/blob/master/bin/std/modules/core"
    }
}