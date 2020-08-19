//#module "compiler1670"
//#unittest error

//#[swag.printbc]
func toto(x: ...)
{
    @assert(@typeof(x[0]).name == "any")
}

#run
{
    toto(5)
}