#global testerror "Err0071"

var READ, TOTO: s32

struct Err {}

func toto() throw
{
    TOTO += 1
}

func read() throw
{
    throw Err{}
}

func titi() throw
{
    defer toto()
    read()
}

#test
{
    catch titi()
    @assert(@err == Err)
}
