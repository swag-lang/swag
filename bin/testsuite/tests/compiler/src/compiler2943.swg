func toto()->u64 => Swag.U64.Max

#test
{
    var x = toto()
    var y = x / cast(f64) Swag.U64.Max
    @assert(y > 0.99 and y < 1.01)
}