//#global skip
struct(X: s32) TOTO
{
    v1: s32
    v: [X] s32
}

func(X: s32) toto()->TOTO
{
    return {0}
}

#test
{
    let s = toto'(4)()
    @assert(#typeof(s.v) == [4] s32)
    let s1 = toto'(5)()
    @assert(#typeof(s1.v) == [5] s32)
}