#module "compiler2141"
#unittest error

func toto(v: string)
{
    @print(v)
}

//#[swag.printbc]
#test
{
    x := @["AA", "BBB", "CCCC", "DDDDD"]
    toto(@spread(x))
}