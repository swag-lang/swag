#import "win32" version = "0.0.0"
#import "xinput" version = "0.0.0"

#run
{
    itf := @compiler()
    cfg := itf.getBuildCfg()
    cfg.moduleVersion = 0
    cfg.moduleRevision = 0
    cfg.moduleBuildNum = 0
}