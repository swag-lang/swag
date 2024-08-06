#global testerror "Err0569"
#test
{
    var x = "toto"
    var y = x[..<0]
    @print(y)
}
