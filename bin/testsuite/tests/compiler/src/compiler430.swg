
//#[Swag.PrintBc]
#test
{
    len("toto")
    len("coucou")
    len([1'u8, 2'u8])
}

func len(buffer: const [..] u8)
{
    var result = 0'u32
    loop buffer do
        result += 1
    @assert(result == @countof(buffer))
}
