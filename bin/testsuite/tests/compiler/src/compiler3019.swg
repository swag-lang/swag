#test
{
    {
        var x, y = true
        var z = x ^ y
        @assert(z == false)
    }

    {
        var x = true
        var y = false
        var z = x ^ y
        @assert(z == true)
    }

    {
        var y = true
        var x = false
        var z = x ^ y
        @assert(z == true)
    }

    {
        var x, y = false
        var z = x ^ y
        @assert(z == false)
    }
}