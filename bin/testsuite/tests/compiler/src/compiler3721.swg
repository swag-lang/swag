func toto() throw
{
    throw "toto"
}

func tata() throw
{
    throw "tata"
}

func read() throw
{
    throw "read"
}

//#[Swag.PrintBc]
func titi() throw
{
    defer
    {
        tata()
    }

    read()
}

#test
{
    catch titi()
    @assert(@errmsg == "tata")
}