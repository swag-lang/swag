func F()->s32
{
    return 666
}

//#[Swag.PrintBc]
#test
{
    let a, b, c = F()
    @assert(a == 666)
    @assert(b == 666)
    @assert(c == 666)
}
