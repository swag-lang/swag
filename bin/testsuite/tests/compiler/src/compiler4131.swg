//#global testerror "Err0820"
#assert(#run @compiler.getBuildCfg().byteCodeOptimizeLevel >= 0)

#[Swag.Safety("nan", false)]
//#[Swag.PrintBc]
#test
{
    let y = Swag.F32.Nan
    let x = @pow(y, 2)
    //@print(x)
}