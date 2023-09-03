#global testerror "Err0688"
#test
{
    var x = "toto"
    var y = x[..<0]
    @print(y)
}
