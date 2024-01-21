#global testerror "Err0745"

#test
{
    var str = "string"
    var tuple = {@dataof(str), 2}
    var ptr = @countof(tuple)
}