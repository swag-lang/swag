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
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "FreeType"
        cfg.backendKind = .Export
    }
}