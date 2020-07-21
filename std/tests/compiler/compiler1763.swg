#[swag.printbc]
#test
{
    x := 3'u32
    y := x % 2
    @print(cast(f64)y)
}