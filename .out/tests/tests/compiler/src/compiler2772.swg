//#[swag.printbc]
func(T) toto()
{
    x := cast(T) 2
    y := cast(T) 1
    @assert(@min(x, y) == 1)
    @assert(@max(x, y) == 2)
    @assert(@min(y, x) == 1)
    @assert(@max(y, x) == 2)
    @assert(@min(x, cast(T) 1) == 1)

    #if @typeof(T) == s8 || @typeof(T) == s16 || @typeof(T) == s32 || @typeof(T) == s64
    {
        @assert(@min(x, cast(T) -1) == -1)
        @assert(@max(cast(T) -2, cast(T) -1) == -1)
        @assert(@min(cast(T) -1, x) == -1)
        @assert(@max(cast(T) -1, x) == x)
    }
}

#test
{
    toto's8()
    toto'u8()
    toto's16()
    toto'u16()
    toto's32()
    toto'u32()
    toto's64()
    toto'u64()
    toto'int()
    toto'uint()

    /*{
        x := 456
        @assert(@min(@max(x, 0), 255) == 255)
    }*/

    /*
    toto'f32()
    toto'f64()*/
}