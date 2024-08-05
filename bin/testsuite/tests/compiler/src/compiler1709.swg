#global testerror "Err0738"

#test
{
    var str = "string"
    var tuple = {@dataof(str), 2}
    var ptr = @countof(tuple)
}