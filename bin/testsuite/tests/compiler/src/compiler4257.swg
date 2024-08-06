#global testerror "Err0599"
#test
{
    var x = "toto"
    var y = x[..<0]
    @print(y)
}
