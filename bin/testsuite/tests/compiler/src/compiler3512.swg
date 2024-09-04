#global testerror "Err0083"

#test
{
    let itf = @compiler()
    itf.compileString("const X = 1")
}