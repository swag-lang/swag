#global testerror
#assert #run @compiler().getBuildCfg().byteCodeOptimizeLevel > 0

#[Swag.Safety("math", false)]
#test
{
    let y = -128's8
    let x = @abs(y)
    @print(x)
}