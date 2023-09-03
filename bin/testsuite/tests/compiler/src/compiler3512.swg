#global testerror "Err0859"

#test
{
    let itf = @compiler()
    itf.compileString("const X = 1")
}