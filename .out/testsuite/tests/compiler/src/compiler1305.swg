#[Swag.macro]
func toto(t: s32)
{
    xx := t
    @assert(t == 100)
    @assert(xx == 100)
}

//#[Swag.printbc]
#test
{
    toto(100)
}