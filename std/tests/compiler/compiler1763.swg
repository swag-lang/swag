#[swag.printbc]
#test
{
    x := 20'u64
    y := x^1
    @print(cast(f64)y)
}