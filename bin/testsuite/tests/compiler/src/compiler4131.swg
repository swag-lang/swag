#global testerror
#assert #run @compiler().getBuildCfg().byteCodeOptimizeLevel > 0

#[Swag.Safety("nan", false)]
//#[Swag.PrintBc]
#test
{
    var y = Swag.F32.Nan
    var x = @pow(y, 2)
    @print(x)
}