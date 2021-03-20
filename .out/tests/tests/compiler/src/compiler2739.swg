//#[swag.printbc]//, swag.optim("", false)]
func toto() throw
{
    try
    {
        errdefer throw "toto"
        throw "titi"
    }
}

#test
{
    catch toto()
    @assert(@err == "toto")
}