#test
{
    loop x: 10'u8
    {
        @assert(@typeof(x) == u8)
    }

    loop x: 10's8
    {
        @assert(@typeof(x) == s8)
    }
}