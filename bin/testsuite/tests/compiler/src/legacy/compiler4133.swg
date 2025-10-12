#global testerror "Saf0017"
#global testerror "Saf0011"

#assert(#run @compiler.getBuildCfg().byteCodeOptimizeLevel >= 0
)

#[Swag.Safety("", "nan", true), Swag.Optimize("", false)]
//#[Swag.PrintBc]
#test
{
    let y = -2.0'f32
    let x = @log(y)
    @print(x)
}
