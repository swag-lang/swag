#global testerror "Saf0023"
#global testerror "Err0428"

#assert #run @compiler().getBuildCfg().byteCodeOptimizeLevel >= 0

#[Swag.Safety("nan", false)]
#[Swag.Optim("", true)]
//#[Swag.PrintBc]
#test
{
    let y = -2.0'f32
    let x = @log(y)
    @print(x)
}