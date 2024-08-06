#global testerror "Err0616"

#test
{
    var str = "string"
    var tuple = {@dataof(str), 2}
    var ptr = @countof(tuple)
}