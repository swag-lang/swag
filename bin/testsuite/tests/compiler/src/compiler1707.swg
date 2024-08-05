#global testerror "Err0378"

#test
{
    var str = "string"
    var tuple = {@dataof(str), 2}
    var ptr = tuple[0]
}