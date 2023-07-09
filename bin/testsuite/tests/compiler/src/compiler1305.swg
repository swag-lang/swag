#[Swag.Macro]
func toto(t: s32)
{
    var xx = t
    @assert(t == 100)
    @assert(xx == 100)
}

//#[Swag.PrintBc]
#test
{
    toto(100)
}