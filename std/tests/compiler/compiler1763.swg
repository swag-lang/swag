#[swag.printbc]
#test
{
    x := 0b10's32
    y := x >> 1
    @print(cast(f64)y)
}