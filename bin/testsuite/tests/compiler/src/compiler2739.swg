struct ErrToto{}
struct ErrTiti{}

//#[Swag.PrintBc]//, Swag.Optim("", false)]
func toto() throw
{
    try
    {
        defer(err) throw ErrToto{}
        throw ErrTiti{}
    }
}

//#[Swag.PrintBc]
#test
{
    catch toto()
    @assert(@err() == ErrToto)
}