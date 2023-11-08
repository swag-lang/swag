#global testerror "Err0401"

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
    try x()
    @assert(@err() == ErrToto)
}