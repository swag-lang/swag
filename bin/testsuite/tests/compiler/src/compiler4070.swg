#test
{
    x := false
    y := ref &x
    while x
    {
        @assert(false)
    }
}