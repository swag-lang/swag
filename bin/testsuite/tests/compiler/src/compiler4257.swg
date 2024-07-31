#global testerror "Err0618"
#test
{
    var x = "toto"
    var y = x[..<0]
    @print(y)
}
