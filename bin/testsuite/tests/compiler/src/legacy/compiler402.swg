//#[Swag.PrintBc]
#test
{
    let c = 0xFFFFFFFF_FFFFFFFF
    #[Swag.CanOverflow(true)]
    let a = c >> 56
    @assert(a == 0xFF)
}
