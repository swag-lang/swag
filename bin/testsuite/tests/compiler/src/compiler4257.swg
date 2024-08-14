#global testerror "Err0505"
#test
{
    var x = "toto"
    var y = x[..<0]
    @print(y)
}
