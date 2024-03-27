//#[Swag.PrintBc]
func toto(val: any)
{
    let err = cast(const &s32) val
    @assert(err == 42)
    let err1 = cast(s32) val
    @assert(err1 == 42)
}

#test
{
    toto(42)
}
