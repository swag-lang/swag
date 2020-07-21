#[swag.printbc]
#test
{
    x := 0b1's32
    y := x << 1
    @print(cast(f64)y)
}