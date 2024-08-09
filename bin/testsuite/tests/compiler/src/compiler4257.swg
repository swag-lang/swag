#global testerror "Err0570"
#test
{
    var x = "toto"
    var y = x[..<0]
    @print(y)
}
