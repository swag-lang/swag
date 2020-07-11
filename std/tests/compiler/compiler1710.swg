#module compiler1710
#unittest error

#test
{
    var str = "string"
    var tuple = {@dataof(str), 2}
    ptr := @kindof(tuple)
}