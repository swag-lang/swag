#test
{
    {
        var x = -1'f64
        x = @abs(x)
        @assert(x == 1)
    }
    {
        var x = -5's8
        x = @abs(x)
        //@print(cast(s64) x)
        @assert(x == 5)
    }
    {
        var x = 1's8
        x = @abs(x)
        //@print(cast(s64) x)
        @assert(x == 1)
    }
    {
        var x = -5's16
        x = @abs(x)
        //@print(cast(s64) x)
        @assert(x == 5)
    }
    {
        var x = -12's32
        x = @abs(x)
        //@print(cast(s64) x)
        @assert(x == 12)
    }
    {
        var x = -256's64
        x = @abs(x)
        //@print(cast(s64) x)
        @assert(x == 256)
    }
    {
        var x = 256's64
        x = @abs(x)
        //@print(cast(s64) x)
        @assert(x == 256)
    }
}
