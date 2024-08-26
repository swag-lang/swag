#global testerror "Err0105"

#test
{
    let itf = @compiler()
    itf.compileString("const X = 1")
}