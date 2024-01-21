#global testerror "Err0747"

#test
{
    var str = "string"
    var tuple = {@dataof(str), 2}
    var ptr = @dataof(tuple)
}