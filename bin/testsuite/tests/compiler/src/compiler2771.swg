//#[Swag.PrintBc, Swag.Optimize("", false)]
#test
{
    let a = 0xFFFFFFFF_FFFFFFFF'u64
    let b = ~a
    @assert(b == 0)
}