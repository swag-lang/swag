var T = true
#test
{
    {
        x := T
        z := x ^ true
        @assert(z == false)
    }
}