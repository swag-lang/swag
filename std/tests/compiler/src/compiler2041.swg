#module "compiler2041"
#unittest error

struct AA
{
    x: s32
}

#test
{
    var x: AA(0)
}