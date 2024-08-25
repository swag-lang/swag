//#global skip
struct ErrToto{}

func toto() throw
{
    throw ErrToto{}
}

//#[Swag.PrintBc, Swag.Optimize("", false)]
#test
{
    var x: func() throw
    x = &toto
    catch x()
    @assert(@err() == ErrToto)
}