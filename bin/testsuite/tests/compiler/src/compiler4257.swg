#global testerror "Err0501"
#test
{
    var x = "toto"
    var y = x[..<0]
    @print(y)
}
