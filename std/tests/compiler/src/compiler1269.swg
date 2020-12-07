#module "compiler1269"
#unittest error

enum A
{
    B
}

func toto(x: s32)
{
}

#test
{
    toto(A.B)
}