#global testerror "Err0084"

#test
{
    let itf = @compiler()
    itf.compileString("const X = 1")
}