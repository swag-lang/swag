const ZFAST_BITS = 9

#[Swag.optim("", false)]
#test
{
    j := 7'u32
    @assert(j < (1 << ZFAST_BITS))
}