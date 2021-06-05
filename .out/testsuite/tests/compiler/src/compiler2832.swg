#global testerror

func toto() -> {x: s32} throw
{
    throw "err"
    return @{1}
}

//#[Swag.printbc]
#test
{
    @assert(assume toto().x == 1)
}