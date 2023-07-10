#test
{
    {
        let x = "abcd"
        let y = x[..1]
        @assert(y == "ab")
    }

    {
        let x = "abcd"
        let y = x[..<2]
        @assert(y == "ab")
    }
}
