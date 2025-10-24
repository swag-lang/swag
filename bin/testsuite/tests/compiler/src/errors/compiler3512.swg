#global testerror "Err0189"

#test
{
    let itf = @compiler
    itf.compileString("const X = 1")
}
