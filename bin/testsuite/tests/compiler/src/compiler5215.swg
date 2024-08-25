#global testerror "Err0003"

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
    try x()
    @assert(@err() == ErrToto)
}