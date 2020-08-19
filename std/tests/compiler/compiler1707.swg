#module "compiler1707"
#unittest error

#test
{
    var str = "string"
    var tuple = {@dataof(str), 2}
    ptr := tuple[0]
}