var T = true
var F = false

#test
{
    {
        let x, y = T
        let z = x ^ y
        @assert(z == false)
    }

    {
        let x = T
        let y = F
        let z = x ^ y
        @assert(z == true)
    }

    {
        let y = T
        let x = F
        let z = x ^ y
        @assert(z == true)
    }

    {
        let x, y = F
        let z = x ^ y
        @assert(z == false)
    }
}
