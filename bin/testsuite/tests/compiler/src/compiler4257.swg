#global testerror "Err0503"
#test
{
    var x = "toto"
    var y = x[..<0]
    @print(y)
}
