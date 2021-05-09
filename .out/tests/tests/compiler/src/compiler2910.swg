const ZFAST_BITS = 9

#[swag.optim("", false)]
#test
{
    j := 7'u32
    @assert(j < (1 << ZFAST_BITS))
}