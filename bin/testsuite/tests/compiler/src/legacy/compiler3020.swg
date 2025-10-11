#test
{
    {
        var x = true
        let y = true
        x ^= y
        @assert(x == false)
    }

    {
        var x = false
        let y = false
        x ^= y
        @assert(x == false)
    }

    {
        var x = false
        let y = true
        x ^= y
        @assert(x == true)
    }

    {
        var x = true
        let y = false
        x ^= y
        @assert(x == true)
    }
}
