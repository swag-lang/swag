#global testerror "Err0071"
var READ, TOTO: s32

func toto() throw
{
    TOTO += 1
}

func read() throw
{
    READ += 1
}

//#[Swag.PrintBc, Swag.Optimize("", false)]
func titi() throw
{
    defer toto()
    read()
}

#test
{
    try titi()
    @assert(READ == 1)
    @assert(TOTO == 1)
}