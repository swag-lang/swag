#global testerror "Saf0027"
#global testerror "Err0427"

#assert #run @compiler().getBuildCfg().byteCodeOptimizeLevel >= 0

#[Swag.Safety("nan", false)]
//#[Swag.PrintBc]
#test
{
    let y = -2.0'f32
    let x = @acos(y)
    @print(x)
}