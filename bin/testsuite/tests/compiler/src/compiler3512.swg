#global testerror "Err0098"

#test
{
    let itf = @compiler()
    itf.compileString("const X = 1")
}