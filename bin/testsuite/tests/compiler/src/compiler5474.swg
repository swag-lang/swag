//#global skip
struct(X: s32) TOTO
{
    v1:     s32
    v:      [X] s32
}

#[Swag.Inline]
func(X: s32) toto()->TOTO'X
{
    return {0}
}

#test
{
    let s = toto'(4)()
    @assert(#typeof(s.v) == #type [4] s32)
}
