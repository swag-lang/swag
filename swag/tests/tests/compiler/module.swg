#import "core" version = "0.0.0" location = "f:/swag/swag/std"

#run
{
    itf := @compiler()
    cfg := itf.getBuildCfg()
    cfg.moduleVersion = 1
    cfg.moduleRevision = 0
    cfg.moduleBuildNum = 0
}