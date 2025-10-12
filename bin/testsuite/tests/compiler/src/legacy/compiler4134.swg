#global testerror "Saf0017"
#global testerror "Saf0013"

#assert(#run @compiler.getBuildCfg().byteCodeOptimizeLevel >= 0
)

#[Swag.Safety(.All, "nan", true)]
//#[Swag.PrintBc]
#test
{
    let y = -2.0'f32
    let x = @log2(y)
    @print(x)
}
