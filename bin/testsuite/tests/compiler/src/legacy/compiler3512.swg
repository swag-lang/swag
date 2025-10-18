#global testerror "Err0404"

#test
{
    let itf = @compiler
    itf.compileString("const X = 1")
}
