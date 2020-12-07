#module "compiler1708"
#unittest error

#test
{
    var str = "string"
    var tuple = {@dataof(str), 2}
    ptr := @dataof(tuple)
}