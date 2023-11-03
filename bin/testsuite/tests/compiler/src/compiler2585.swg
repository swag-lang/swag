struct ErrToto{}

func toto() throw
{
    throw ErrToto{}
}

//#[Swag.PrintBc]
#test
{
    var x: func() throw
    x = &toto
    catch x()
    @assert(@err() == ErrToto)
}