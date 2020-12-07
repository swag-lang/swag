#module "compiler1525"
#unittest error

func titi()
{

}

func(T) toto(x: T)
{

}

#test
{
    toto(1)
    toto(1'u32)
    //toto(1)
}