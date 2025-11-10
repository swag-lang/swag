func toto(x: const &string)
{
    let ptr = cast(const [*] u8) @dataof(x)
    @assert(ptr[0] == '4')
    @assert(ptr[1] == '2')
}

#test
{
    toto("42")
}
