#global testerror "Saf0008"
#assert #run @compiler().getBuildCfg().byteCodeOptimizeLevel >= 0

#[Swag.Safety("math", true)]
#test
{
    const y = -128's8
    let x = @abs(y)
    @print(x)
}