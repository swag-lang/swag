//#global skip
struct(X: s32) TOTO
{
    v1:     s32
    v:      [X] s32
}

func(X: s32) toto()
{
    var t: TOTO
    @assert(#typeof(t.v) == #type [4] s32)
    //return {0}
}

#test
{
    toto'(4)()
}
