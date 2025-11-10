struct ErrTiti {}
struct ErrTata {}

func titi() throw
{
    throw ErrTiti{}
}

func tata() throw
{
    defer catch titi()
    throw ErrTata{}
}

#test
{
    catch tata()
}
