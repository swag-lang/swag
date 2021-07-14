//#[Swag.PrintBc]//, Swag.Optim("", false)]
func toto() throw
{
    try
    {
        errdefer throw "toto"
        throw "titi"
    }
}

//#[Swag.PrintBc]
#test
{
    catch toto()
    @assert(@err == "toto")
}