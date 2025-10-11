struct ErrToto {}
struct ErrRead {}

var READ, TOTO: s32

func toto() throw
{
    throw ErrToto{}
}

func read() throw
{
    throw ErrRead{}
}

//#[Swag.PrintBc]
func titi() throw
{
    defer catch toto()
    read()
}

#test
{
    catch titi()
    @assert(@err == ErrRead)
}
