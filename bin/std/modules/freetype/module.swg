// 'freetype' module is a wrapper for the great Freetype 2 library.
// Current supported version is 2.10.4
//
// Portions of this software are copyright.
// ©2021 The FreeType Project (www.freetype.org).
// All rights reserved.

#dependencies
{
    #import "win32"

    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion = 2
        cfg.moduleRevision = 10
        cfg.moduleBuildNum = 4
        cfg.moduleNamespace = "FreeType"
        cfg.backendKind = .Export
    }
}