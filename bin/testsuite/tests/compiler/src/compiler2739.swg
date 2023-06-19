//#[Swag.PrintBc]//, Swag.Optim("", false)]
func toto() throw
{
    try
    {
        defer(err) throw "toto"
        throw "titi"
    }
}

//#[Swag.PrintBc]
#test
{
    catch toto()
    @assert(@errmsg == "toto")
}