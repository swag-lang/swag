#global testerror "Err0094"

#test
{
    let itf = @compiler()
    itf.compileString("const X = 1")
}