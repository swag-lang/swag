//#global skip
func(A, B) toto(x: {A, B})
{
    @assert(x.item0 == 1)
    @assert(x.item1 == 2)
}

#test
{
    toto'(s32, s32)({1, 2})
}