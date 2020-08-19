#module "compiler1526"
#unittest error

func(T) toto(x: T)
{

}

#test
{
    toto!bool(1.0)
}