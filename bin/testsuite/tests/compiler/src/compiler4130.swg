#global testerror "Err0605"
#assert #run @compiler().getBuildCfg().byteCodeOptimizeLevel > 0

#[Swag.Safety("math", false)]
#test
{
    const y = -128's8
    let x = @abs(y)
    @print(x)
}