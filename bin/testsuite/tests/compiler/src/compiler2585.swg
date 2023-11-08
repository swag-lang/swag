#global skip
struct ErrToto{}

func toto() throw
{
    @breakpoint()
    throw ErrToto{}
}

#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var x: func() throw
    x = &toto
    try x()
    @assert(@err() == ErrToto)
}