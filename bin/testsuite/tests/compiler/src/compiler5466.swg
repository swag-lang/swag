struct A {}

#[Swag.Inline]
func toto(what: typeinfo, y: any)->bool
{
    return y == what
}

#test
{
    @assert(toto(bool, true))
    /*    @assert(toto(0, s32))
    @assert(!toto("0", s32))
    @assert(toto("0", string))
    @assert(toto(A{}, A))*/
}
