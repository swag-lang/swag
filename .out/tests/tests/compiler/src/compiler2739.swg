//#[swag.printbc]//, swag.optim("", false)]
func toto() throw
{
    try
    {
        errdefer throw "toto"
        throw "titi"
    }
}

//#[swag.printbc]
#test
{
    catch toto()
    @assert(@err == "toto")
}