const ZFAST_BITS = 9

#[Swag.Optim("", false)]
#test
{
    j := 7'u32
    @assert(j < (1 << ZFAST_BITS))
}