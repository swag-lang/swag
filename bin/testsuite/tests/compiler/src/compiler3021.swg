var T = true
var F = false

#test
{
    {
        var x, y = T
        var z = x ^ y
        @assert(z == false)
    }

    {
        var x = T
        var y = F
        var z = x ^ y
        @assert(z == true)
    }

    {
        var y = T
        var x = F
        var z = x ^ y
        @assert(z == true)
    }

    {
        var x, y = F
        var z = x ^ y
        @assert(z == false)
    }
}