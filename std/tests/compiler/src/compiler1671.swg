//#module "compiler1670"
//#unittest error

//#[swag.printbc]
func toto(x: ...)
{
    @assert(@kindof(x[0]).name == "s32")
}

#run
{
    toto(5)
}