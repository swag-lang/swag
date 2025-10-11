//#[Swag.PrintBc]
#test
{
    len("toto")
    len("coucou")
    len([1'u8, 2'u8])
}

//#[Swag.PrintBc]
func len(buffer: const [..] u8)
{
    {
        var result = 0'u32
        for buffer do
            result += 2
        @assert(result == 2 * @countof(buffer))
    }

    {
        var result = 0'u32
        for buffer do
            result += 1
        @assert(result == 1 * @countof(buffer))
    }
}
