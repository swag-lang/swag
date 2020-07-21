#[swag.printbc]
#test
{
    x := 0b10's64
    y := x >> 1
    @print(cast(f64)y)
}