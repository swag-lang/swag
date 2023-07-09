var T = true
#test
{
    {
        var x = T
        var z = x ^ true
        @assert(z == false)
    }
}