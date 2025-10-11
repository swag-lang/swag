#global testerror "Err0086"

#test
{
    let itf = @compiler
    itf.compileString("const X = 1")
}
