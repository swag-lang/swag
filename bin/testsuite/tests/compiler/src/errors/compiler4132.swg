#global testerror "Saf0009"

#assert(#run @compiler.getBuildCfg().byteCodeOptimizeLevel >= 0
)

#[Swag.Safety(.All, .All, true)]
//#[Swag.PrintBc]
#test
{
    let y = -2.0'f32
    let x = @acos(y)
    @print(x)
}
