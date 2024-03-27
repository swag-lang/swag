//#[Swag.PrintBc]
func toto(x: any)
{
    @assert(@kindof(x) == string)
}

#test
{
    let x = "toto"
    toto(x)
}
