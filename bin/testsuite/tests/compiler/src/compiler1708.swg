#global testerror "Err0546"

#test
{
    var str = "string"
    var tuple = {@dataof(str), 2}
    var ptr = @dataof(tuple)
}