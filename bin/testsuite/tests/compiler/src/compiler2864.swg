#global testerror
#assert #run @compiler().getBuildCfg().byteCodeOptimizeLevel > 0

//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
#[Swag.Safety("math", true)]
#test
{
    /*x := 0xFFFFFFFF'u32
    var y: s32
    y = x + 1
    @print(cast(s64) y)*/
    x := -1.0
    y := @sqrt(x)
}