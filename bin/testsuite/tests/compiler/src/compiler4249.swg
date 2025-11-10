#test
{
    {
        let x = "abcd"
        let y = x[ to 1]
        @assert(y == "ab")
    }

    {
        let x = "abcd"
        let y = x[ until 2]
        @assert(y == "ab")
    }
}
