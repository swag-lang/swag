#global testerror
#global if @compiler().getBuilfCfg().byteCodeOptimizeLevel > 0

#[Swag.Safety("math", false)]
#test
{
    y := -128's8
    x := @abs(y)
    @print(x)
}