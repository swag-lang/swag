func toto()->u64 => Swag.U64.Max

#test
{
    let x = toto()
    let y = x / cast(f64) Swag.U64.Max
    @assert(y > 0.99 and y < 1.01)
}
