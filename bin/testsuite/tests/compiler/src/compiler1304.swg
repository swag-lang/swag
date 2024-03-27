#[Swag.Macro]
func toto(t: s32)
{
    @assert(t == 100)
}

#test
{
    toto(100)
}