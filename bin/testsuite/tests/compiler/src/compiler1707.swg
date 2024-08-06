#global testerror "Err0365"

#test
{
    var str = "string"
    var tuple = {@dataof(str), 2}
    var ptr = tuple[0]
}