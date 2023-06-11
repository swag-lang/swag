var READ, TOTO: s32

func toto() throw
{
    TOTO += 1
}

func read() throw
{
    throw "read"
}

func titi() throw
{
    defer toto()
    read()
}

#test
{
    catch titi()
    @assert(@errmsg == "read")
}