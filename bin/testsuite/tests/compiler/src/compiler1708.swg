#global testerror "Err0568"

#test
{
    var str = "string"
    var tuple = {@dataof(str), 2}
    var ptr = @dataof(tuple)
}