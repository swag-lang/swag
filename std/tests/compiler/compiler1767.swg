#module "compiler1767"
#unittest error

//#[swag.printbc]
func toto(x: [2] s32)
{
    //x[0] = 66
    //@print(cast(s64) x[0])
}

//#[swag.printbc]
#test
{
    toto([1, 2])
}