#module "compiler1569"
#unittest error

struct A
{
    x: s32 = 1
}

func toto(val: A)
{
    var y: A = move val
}
