//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var a = 0xFFFFFFFF_FFFFFFFF'u64
    var b = ~a
    @assert(b == 0)
}