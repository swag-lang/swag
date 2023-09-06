//#global testerror
func(A, B) toto(x: {a: A, b: B, c: s32})
{
    @assert(x.item0 == 1)
    @assert(x.item1 == true)
    @assert(x.a == 1)
    @assert(x.b == true)
    @assert(x.c == 55)
}

#test
{
    toto'(s32, bool)({b: true, a: 1, c: 55})
}