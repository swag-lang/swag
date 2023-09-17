#global testerror "Saf0021"
#global testerror "Saf0025"

#assert #run @compiler().getBuildCfg().byteCodeOptimizeLevel >= 0

#[Swag.Safety("nan", true)]
//#[Swag.PrintBc]
#test
{
    let y = -2.0'f32
    let x = @log10(y)
    @print(x)
}