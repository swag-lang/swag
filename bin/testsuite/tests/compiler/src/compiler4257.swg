#global testerror "Err0612"
#test
{
    var x = "toto"
    var y = x[..<0]
    @print(y)
}
