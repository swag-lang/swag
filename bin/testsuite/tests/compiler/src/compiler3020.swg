#test
{
    {
        var x, y = true
        x ^= y
        @assert(x == false)
    }

    {
        var x, y = false
        x ^= y
        @assert(x == false)
    }

    {
        var x = false
        var y = true
        x ^= y
        @assert(x == true)
    }

    {
        var x = true
        var y = false
        x ^= y
        @assert(x == true)
    }
}