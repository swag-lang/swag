#module "compiler1706"
#unittest error

#test
{
    var str = "string"
    var tuple = {@dataof(str), 2}
    tuple[0] = 0
}