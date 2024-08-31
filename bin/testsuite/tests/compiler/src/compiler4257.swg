#global testerror "Err0544"
#test
{
    var x = "toto"
    var y = x[..<0]
    @print(y)
}
