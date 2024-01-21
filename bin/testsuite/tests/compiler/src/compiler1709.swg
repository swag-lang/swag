#global testerror "Err0742"

#test
{
    var str = "string"
    var tuple = {@dataof(str), 2}
    var ptr = @countof(tuple)
}