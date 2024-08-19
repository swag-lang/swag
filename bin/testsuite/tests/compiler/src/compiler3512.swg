#global testerror "Err0087"

#test
{
    let itf = @compiler()
    itf.compileString("const X = 1")
}