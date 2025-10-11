struct ErrTata {}
struct ErrTiti {}

func tata() throw
{
    throw ErrTata{}
}

func toto()
{
    catch tata()
    @assert(@err == ErrTata)
}

func titi() throw
{
    defer toto()
    throw ErrTiti{}
}

#test
{
    catch titi()
    @assert(@err == ErrTiti)
}
