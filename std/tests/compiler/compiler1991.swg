func(T) way(x1, x2: T)
{
    var s: s32

    s = x1 <=> x2
    @assert(s == -1)
    s = x2 <=> x1
    @assert(s == 1)
    s = x1 <=> x1
    @assert(s == 0)
}

#[swag.optimizebc(0)]
#test
{
    way(10's32, 20's32)
    way(10'u32, 20'u32)
    way(-10's32, -5's32)
    way(10's64, 20's64)
    way(-10's64, -5's64)
    way(10'u64, 20'u64)

    way(10'f32, 20'f32)
    way(-10'f32, -5'f32)
    way(10'f64, 20'f64)
    way(-10'f64, -5'f64)
}