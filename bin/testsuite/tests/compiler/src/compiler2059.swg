//#[Swag.PrintBc]
func toto(x: any)
{
    @assert(@kindof(x) == string)
}

#test
{
    var x = "toto"
    toto(x)
}