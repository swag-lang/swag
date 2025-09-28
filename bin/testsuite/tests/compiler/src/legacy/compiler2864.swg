#global testerror "Saf0014"
#assert(#run @compiler.getBuildCfg().byteCodeOptimizeLevel >= 0)

//#[Swag.PrintBc]
//#[Swag.Optimize("", false)]
#[Swag.Safety("math", true)]
#test
{
    /*var x = 0xFFFFFFFF'u32
    var y: s32
    y = x + 1
    @print(cast(s64) y)*/
    let x = -1.0
    let y = @sqrt(x)
}