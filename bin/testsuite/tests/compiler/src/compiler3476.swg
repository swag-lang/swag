#test
{
    var x: func(bool, ...) = func(x: bool, y: ...)
    {
        @assert(x == true)
        @assert(cast(s32) y[0] == 4)
    }
    var b: bool = true

    b.x(4)
}