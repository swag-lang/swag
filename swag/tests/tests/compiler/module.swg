//#import "toto"

#run
{
    itf := @compiler()
    cfg := itf.getBuildCfg()
    cfg.moduleVersion = "1.0.0"
}