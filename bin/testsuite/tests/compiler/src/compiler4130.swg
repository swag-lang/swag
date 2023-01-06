#global testerror
#assert #run @compiler().getBuildCfg().byteCodeOptimizeLevel > 0

#[Swag.Safety("math", false)]
#test
{
    y := -128's8
    x := @abs(y)
    @print(x)
}