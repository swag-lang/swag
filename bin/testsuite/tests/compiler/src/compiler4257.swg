#global testerror "Err0537"
#test
{
    var x = "toto"
    var y = x[..<0]
    @print(y)
}
