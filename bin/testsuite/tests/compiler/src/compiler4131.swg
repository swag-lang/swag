#global testerror
#global if #run @compiler().getBuildCfg().byteCodeOptimizeLevel > 0

#[Swag.Safety("nan", false)]
//#[Swag.PrintBc]
#test
{
    y := Swag.F32.Nan
    x := @pow(y, 2)
    @print(x)
}