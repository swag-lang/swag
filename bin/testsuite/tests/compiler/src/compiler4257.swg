#global testerror "Err0613"
#test
{
    var x = "toto"
    var y = x[..<0]
    @print(y)
}
