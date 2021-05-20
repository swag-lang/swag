//#[Swag.printbc]//, Swag.optim("", false)]
func toto() throw
{
    try
    {
        errdefer throw "toto"
        throw "titi"
    }
}

//#[Swag.printbc]
#test
{
    catch toto()
    @assert(@err == "toto")
}