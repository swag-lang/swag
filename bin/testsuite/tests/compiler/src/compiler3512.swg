#global testerror

#test
{
    var itf = @compiler()
    itf.compileString("const X = 1")
}