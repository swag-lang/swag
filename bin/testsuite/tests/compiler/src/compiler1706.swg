#global testerror "Err0378"

#test
{
    var str = "string"
    var tuple = {@dataof(str), 2}
    tuple[0] = 0
}