//#import "toto"

#run
{
    @print("toto")
    itf := @compiler()
    cfg := itf.getBuildCfg()
    cfg.moduleVersion = "1.0.0"
}