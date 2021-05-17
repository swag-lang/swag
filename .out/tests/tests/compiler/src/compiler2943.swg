func toto()->u64 => swag.U64.Max

#test
{
    x := toto()
    y := x / cast(f64) swag.U64.Max
    @print(y)
}