#global testerror
#assert #run @compiler().getBuildCfg().byteCodeOptimizeLevel > 0

#[Swag.Safety("nan", false)]
//#[Swag.PrintBc]
#test
{
    y := -2.0'f32
    x := @log10(y)
    @print(x)
}