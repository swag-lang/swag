struct ErrToto {}
struct ErrTiti {}

//#[Swag.PrintBc]//, Swag.Optimize(.All, false)]
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
