//#[Swag.PrintBc]
func toto(x: const &const [..] s32)
{
    let ptr = cast(const [*] s32) @dataof(x)
    @assert(ptr[0] == 4)
    @assert(ptr[1] == 2)
}

//#[Swag.PrintBc]
#test
{
    let a: const [..] s32 = [4, 2]
    toto(a)
}
