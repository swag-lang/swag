struct(A, B) Foozle
{
    a: A
    b: B
}

var G = 0
func(T, TT) yes_auto2(x: T, y: TT)
{
    if(@typeof(x.a) == s32) G |= 1
    if(@typeof(y.a) == bool) G |= 2
}

#test
{
    var fooz1: Foozle'(s32, f32)
    var fooz2: Foozle'(bool, @typeof(fooz1))
    yes_auto2(fooz1, fooz2)
    @assert(G == 3)
}