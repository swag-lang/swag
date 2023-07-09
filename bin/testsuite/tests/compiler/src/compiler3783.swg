#test
{
    {
        var x = 0's8
        x -= 1
        var y: s16 = x
        @assert(y == -1)
    }

    {
        var x = 0's8
        x -= 1
        var y: s32 = x
        @assert(y == -1)
    }

    {
        var x = 0's8
        x -= 1
        var y: s64 = x
        @assert(y == -1)
    }

    {
        var x = 0's32
        x -= 1
        var y: s64 = x
        @assert(y == -1)
    }

    {
        var x = 0's16
        x -= 1
        var y: s64 = x
        @assert(y == -1)
    }
}