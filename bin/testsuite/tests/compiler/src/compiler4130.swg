#global testerror
#assert #run @compiler().getBuildCfg().byteCodeOptimizeLevel > 0

#[Swag.Safety("math", false)]
#test
{
    var y = -128's8
    var x = @abs(y)
    @print(x)
}