#global testerror "Err0516"
#test
{
    var x = "toto"
    var y = x[..<0]
    @print(y)
}
