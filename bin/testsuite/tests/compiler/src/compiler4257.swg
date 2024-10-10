#global testerror "Err0504"
#test
{
    var x = "toto"
    var y = x[..<0]
    @print(y)
}
