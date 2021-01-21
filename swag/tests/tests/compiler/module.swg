#import "toto"

#run
{
    @print("toto")
    itf := @compiler()
    cfg := itf.getBuildCfg()
    //cfg.backendLLVM.outputIR = true
}