#test
{
    // s32 version
    #[Swag.Overload]
    func(T) isNull(x: T)->bool
        where #typeof(T) == s32
    {
        return x == 0
    }

    // f32/f64 version
    #[Swag.Overload]
    func(T) isNull(x: T)->bool
        where #typeof(T) == f32 or #typeof(T) == f64
    {
        return @abs(x) < 0.01
    }

    @assert(isNull(0's32))
    @assert(isNull(0.001'f32))
}
