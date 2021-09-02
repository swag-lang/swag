var T = true
var F = false

#test
{
    {
        x, y := T
        z := x ^ y
        @assert(z == false)
    }

    {
        x := T
        y := F
        z := x ^ y
        @assert(z == true)
    }

    {
        y := T
        x := F
        z := x ^ y
        @assert(z == true)
    }

    {
        x, y := F
        z := x ^ y
        @assert(z == false)
    }
}