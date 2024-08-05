#global testerror "Err0610"
#test
{
    var x = "toto"
    var y = x[..<0]
    @print(y)
}
