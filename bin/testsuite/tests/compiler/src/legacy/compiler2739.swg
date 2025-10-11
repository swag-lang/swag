struct ErrToto {}
struct ErrTiti {}

//#[Swag.PrintBc]//, Swag.Optimize("", false)]
func toto() throw
{
    try
    {
        defer #noerr throw ErrToto{}
        throw ErrTiti{}
    }
}

//#[Swag.PrintBc]
#test
{
    catch toto()
    @assert(@err == ErrTiti)
}
