//#[Swag.printbc]
func toto(x: any)
{
    @assert(@kindof(x) == string)
}

#test
{
    x := "toto"
    toto(x)
}