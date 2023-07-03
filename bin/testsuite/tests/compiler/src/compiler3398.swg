//#global testerror
func(A, B) toto(x: {a: A, b: B})
{
    @assert(x.item0 == 1)
    @assert(x.item1 == true)
    @assert(x.a == 1)
    @assert(x.b == true)
}

#test
{
    toto'(s32, bool)({b = true, a = 1})
}