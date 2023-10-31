var READ, TOTO: s32

func toto() throw
{
    throw "toto"
}

func read() throw
{
    throw "read"
}

//#[Swag.PrintBc]
func titi() throw
{
    defer toto()
    read()
}

#test
{
    catch titi()
    @assert(cast(string) @err() == "toto")
}