#global testerror "Saf0016"
#global testerror "Saf0011"

#assert #run @compiler().getBuildCfg().byteCodeOptimizeLevel >= 0

#[Swag.Safety("nan", true), Swag.Optim("", false)]
//#[Swag.PrintBc]
#test
{
    let y = -2.0'f32
    let x = @log(y)
    @print(x)
}
