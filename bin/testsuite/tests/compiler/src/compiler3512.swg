#global testerror "Err0106"

#test
{
    let itf = @compiler()
    itf.compileString("const X = 1")
}