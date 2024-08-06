#global testerror "Err0099"

#test
{
    let itf = @compiler()
    itf.compileString("const X = 1")
}