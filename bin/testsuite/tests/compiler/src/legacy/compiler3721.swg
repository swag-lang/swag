struct ErrToto {}
struct ErrTata {}
struct ErrRead {}

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

//#[Swag.PrintBc]
func titi() throw
{
    defer
    {
        catch tata()
    }

    read()
}

#test
{
    catch titi()
    @assert(@err == ErrRead)
}
