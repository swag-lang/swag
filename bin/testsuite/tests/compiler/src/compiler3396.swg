#global testerror "Err0724"
func(A, B) toto(x: {a: A, b: B})
{
    @assert(x.item0 == 1)
    @assert(x.item1 == 2)
    @assert(x.a == 1)
    @assert(x.b == 2)
}

#test
{
    toto'(s32, s32)({x: 1, 2})
}