#global testerror "Saf0025"
#global testerror "Err0424"

#assert #run @compiler().getBuildCfg().byteCodeOptimizeLevel >= 0

#[Swag.Safety("nan", false)]
//#[Swag.PrintBc]
#test
{
    let y = -2.0'f32
    let x = @log10(y)
    @print(x)
}