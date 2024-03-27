#global testerror "Err0058"

struct ErrToto
{
}
struct ErrTata
{
}
struct ErrRead
{
}

func toto() throw
{
    throw ErrToto{}
}

func tata() throw
{
    throw ErrTata{}
}

func read() throw
{
    throw ErrRead{}
}

func titi() throw
{
    defer toto()
    defer tata()
    read()
}

#test
{
    catch titi()
    @assert(@err() == ErrTata)
}
