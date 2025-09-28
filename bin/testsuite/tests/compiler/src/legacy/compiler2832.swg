#global skip
#global testerror "ErrInvalid"

struct ErrInvalid{}

func toto() -> {x: s32} throw
{
    throw ErrInvalid{}
    return {1}
}

#[Swag.PrintBc]
#test
{
    @assert(try toto().x == 0)
}