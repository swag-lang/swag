#global testerror "Err0567"
#test
{
    var x = "toto"
    var y = x[..<0]
    @print(y)
}
