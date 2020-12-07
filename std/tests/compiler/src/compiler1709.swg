#module "compiler1709"
#unittest error

#test
{
    var str = "string"
    var tuple = {@dataof(str), 2}
    ptr := @countof(tuple)
}