#import "core" version = "1.0.0" location = "f:/swag/swag/std"

#run
{
    itf := @compiler()
    cfg := itf.getBuildCfg()
    cfg.moduleVersion = "1.0.0"
}