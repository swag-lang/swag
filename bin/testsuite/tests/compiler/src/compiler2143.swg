// A simple point with 2 float coordinates X and Y
struct(T) P
{
    const XX: T = 1
    x, y: T
}

#test
{
    var p: P's32
    @assert(p.XX == 1)
    @assert(#typeof(p.XX) == s32)

    var p1: P'f64
    @assert(#typeof(p1.XX) == f64)
}