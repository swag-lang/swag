#global testerror "Err0112"

#test
{
    let itf = @compiler()
    itf.compileString("const X = 1")
}