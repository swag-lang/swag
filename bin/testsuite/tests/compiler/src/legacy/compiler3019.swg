#test
{
    {
        let x, y = true
        let z = x ^ y
        @assert(z == false)
    }

    {
        let x = true
        let y = false
        let z = x ^ y
        @assert(z == true)
    }

    {
        let y = true
        let x = false
        let z = x ^ y
        @assert(z == true)
    }

    {
        let x, y = false
        let z = x ^ y
        @assert(z == false)
    }
}
