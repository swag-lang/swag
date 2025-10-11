struct A {}

func toto(y: any, what: typeinfo)->bool
{
    return y == what
}

#test
{
    @assert(toto(true, bool))
    @assert(toto(0, s32))
    @assert(!toto("0", s32))
    @assert(toto("0", string))
    @assert(toto(A{}, A))
}
