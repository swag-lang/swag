struct(A, B) Foozle
{
    a: A
    b: B
}

var G = 0
func yes_auto(x: Foozle)
{
    if(@typeof(x.a) == s32) G |= 1
    if(@typeof(x.a) == bool) G |= 2
}

#test
{
    var fooz1: Foozle'(s32, f32)
    var fooz2: Foozle'(bool, @typeof(fooz1))
    yes_auto(fooz1)
    yes_auto(fooz2)
    @assert(G == 3)
}