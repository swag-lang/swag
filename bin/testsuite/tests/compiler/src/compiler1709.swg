#global testerror "Err0567"

#test
{
    var str = "string"
    var tuple = {@dataof(str), 2}
    var ptr = @countof(tuple)
}