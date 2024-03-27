//#global skip
struct ErrToto{}

func toto() throw
{
    throw ErrToto{}
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var x: func() throw
    x = &toto
    catch x()
    @assert(@err() == ErrToto)
}