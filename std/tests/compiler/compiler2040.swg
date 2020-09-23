#module "compiler2040"
#unittest error

struct AA
{
    x: s32
}

#test
{
    x := AA(0)
}