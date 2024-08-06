#global testerror "Err0571"
#test
{
    var x = "toto"
    var y = x[..<0]
    @print(y)
}
