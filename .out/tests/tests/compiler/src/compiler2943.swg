func toto()->u64 => swag.U64.Max

#test
{
    x := toto()
    y := x / cast(f64) swag.U64.Max
    @assert(y > 0.99 and y < 1.01)
}