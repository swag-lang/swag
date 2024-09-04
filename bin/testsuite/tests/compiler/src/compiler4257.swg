#global testerror "Err0493"
#test
{
    var x = "toto"
    var y = x[..<0]
    @print(y)
}
