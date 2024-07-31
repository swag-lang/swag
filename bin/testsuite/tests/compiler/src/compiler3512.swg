#global testerror "Err0109"

#test
{
    let itf = @compiler()
    itf.compileString("const X = 1")
}