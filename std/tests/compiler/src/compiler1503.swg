#module "compiler1502"
#unittest error

func titi()->(s32, s32)
{
    return {0, 1}
}

#test
{
    var (toto[512], titi) = u16
}